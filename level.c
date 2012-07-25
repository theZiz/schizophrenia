/*
 The contents of this file are subject to the Mozilla Public License
 Version 1.1 (the "License"); you may not use this file except in
 compliance with the License. You may obtain a copy of the License at
 http://www.mozilla.org/MPL/

 Software distributed under the License is distributed on an "AS IS"
 basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 License for the specific language governing rights and limitations
 under the License.

 Alternatively, the contents of this file may be used under the terms
 of the GNU Lesser General Public license (the  "LGPL License"), in which case the
 provisions of LGPL License are applicable instead of those
 above.

 For feedback and questions about our Files and Projects please mail us,
 Janek Schäfer (foxblock) , foxblock_gmail_com
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/

#include "level.h"

pLevelObjectGroup createGroup(pLevelObjectGroup *firstGroup, LevelObjectGroupType type)
{
	pLevelObjectGroup group = (pLevelObjectGroup)malloc(sizeof(tLevelObjectGroup));
	group->type = type;
	group->firstObject = NULL;
	if (*firstGroup)
	{
		(*firstGroup)->prev->next = group;
		group->prev = (*firstGroup)->prev;
		group->next = (*firstGroup);
		(*firstGroup)->prev = group;
	}
	else
	{
		group->next = group;
		group->prev = group;
	}
	(*firstGroup) = group;
	return group;
}

pLevelObject createObject(pLevelObjectGroup group,LevelObjectType type)
{
	pLevelObject obj = (pLevelObject)malloc(sizeof(tLevelObject));
	obj->type = type;
	obj->animation = NULL;
	if (group->firstObject)
	{
		group->firstObject->prev->next = obj;
		obj->prev = group->firstObject->prev;
		obj->next = group->firstObject;
		group->firstObject->prev = obj;
	}
	else
	{
		obj->next = obj;
		obj->prev = obj;
	}
	group->firstObject = obj;
	return obj;
}

void allocLayer(pLayer layer,int width, int height)
{
	layer->tile = (spSpritePointer*)malloc(sizeof(spSpritePointer)*width*height);
	memset(layer->tile,0,width*height*sizeof(spSpritePointer));
	layer->width = width;
	layer->height = height;
}

#define READ_TIL_TAG_BEGIN end = spReadUntil(file,buffer,65536,'<'); if (end) break;
#define READ_TIL_TAG_END end = spReadUntil(file,buffer,65536,'>'); if (end) break;

int getHexSign(char sign)
{
	if (sign >= '0' && sign <= '9')
		return sign-'0';
	if (sign >= 'A' && sign <= 'F')
		return sign-'A'+10;
	if (sign >= 'a' && sign <= 'f')
		return sign-'a'+10;
	return 0;
}

Uint16 hex2color(char* value)
{
	if (strlen(value) < 6)
		return 0;
	int r = getHexSign(value[0])*16+getHexSign(value[1]);
	int g = getHexSign(value[2])*16+getHexSign(value[3]);
	int b = getHexSign(value[4])*16+getHexSign(value[5]);
	return spGetRGB(r,g,b);
}

pLevel loadLevel(char* filename)
{
	SDL_RWops *file = SDL_RWFromFile(filename, "rt");
	if (!file)
	{
		printf("Level \"%s\" not found. We're all gone die!\n",filename);
		return NULL;
	}
	//Creating a new, empty level
	pLevel level = (pLevel)malloc(sizeof(tLevel));
	level->layer.physic.tile = NULL;
	level->layer.background.tile = NULL;
	level->layer.player.tile = NULL;
	level->layer.foreground.tile = NULL;
	level->camera.x = 0;
	level->camera.y = 0;
	level->backgroundColor = 65535; //white
	
	//Some values, which will be read and used later
	int width = 0;
	int height = 0;
	
	char buffer[65536];
	//Reading to the begin of the first tag
	spReadUntil(file,buffer,65536,'<');
	//Reading the first tag
	spReadUntil(file,buffer,65536,'>');
	if (buffer[0] == '?')
		printf("Reading first line %s - Everything as expected\n",buffer);
	else
	{
		printf("Reading first line %s - What the hell is this?\n",buffer);
		SDL_RWclose(file);
		return NULL;
	}
	//Reading the first level specific tag
	spReadUntil(file,buffer,65536,'<');
	spReadUntil(file,buffer,65536,'>');
	if (strstr(buffer,"map"))
	{
		printf("Reading level map\n");
		//Parsing parameters of "map"
		char *begin = strchr(buffer,' ');
		while (1)
		{
			if (!begin)
				break;
			begin++; // sign after ' '
			char* end = strchr(begin,' ');
			if (end)
				end[0] = 0;
			else
				break;
			//the parameter is now in "begin"
			//we search only width and height
			if (strstr(begin,"width")==begin) //width at the BEGIN of the string!
			{
				char* number_begin = strchr(begin,'\"');
				number_begin++; //now the begin of the number
				width = atoi(number_begin);
				printf("Level width is %i\n",width);
			}
			else
			if (strstr(begin,"height")==begin) //height at the BEGIN of the string!
			{
				char* number_begin = strchr(begin,'\"');
				number_begin++; //now the begin of the number
				height = atoi(number_begin);
				printf("Level height is %i\n",height);
			}			
			begin = end;
		}
		//Now reading everything, which is between <map …> and </map>
		//Creating the layers and setting them to NULL.
		printf("Creating layers\n");
		allocLayer(&(level->layer.physic),width,height);
		allocLayer(&(level->layer.background),width,height);
		allocLayer(&(level->layer.player),width,height);
		allocLayer(&(level->layer.foreground),width,height);
		int end = 0;
		while (!end)
		{
			READ_TIL_TAG_BEGIN
			READ_TIL_TAG_END
			//possibilites are: properties, tileset, layer, objectgroup or /map
			if (strstr(buffer,"properties") == buffer)
			{
				//Reading the propertes. Possibilites are: "background color"
				READ_TIL_TAG_BEGIN
				READ_TIL_TAG_END
				if (strstr(buffer,"property") == buffer)
				{
					char* property = strstr(buffer,"name");
					property = strchr(property,'\"');
					property++;
					char* end = strchr(property,'\"');
					end[0] = 0;
					printf("Found the property \"%s\"\n",property);
					if (strcmp(property,"background color") == 0)
					{
						end++;
						char* value = strstr(end,"value");
						value = strstr(value,"#");
						end = strchr(value,'\"');
						end[0] = 0;
						level->backgroundColor = hex2color(value);
						printf("Set the background color to %s (%i)\n",value,level->backgroundColor);
					}
				}
			}
		}
	}
	else
	{
		printf("Expected \"map …\", not \"%s\".\n",buffer);
		SDL_RWclose(file);
		return NULL;
	}
	
	SDL_RWclose(file);
	return level;
}

void drawLevel(pLevel level)
{
	//TODO: Implement
}

void deleteLevel(pLevel level)
{
	if (level->layer.physic.tile)
		free(level->layer.physic.tile);
	if (level->layer.background.tile)
		free(level->layer.background.tile);
	if (level->layer.player.tile)
		free(level->layer.player.tile);
	if (level->layer.foreground.tile)
		free(level->layer.foreground.tile);
	
}
