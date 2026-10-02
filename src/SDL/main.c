#include "game.h"
#include "backend.h"
#include <SDL3/SDL.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

/* Host configuration, not reconstructed game logic. The configuration lives
 * beside the preservation executable; original data may live elsewhere. */
#define CHI_CONFIG_FILE "CrescentHawksInception.ini"
#define CHI_ASSET_DIRECTORY_ENV "CHI_ASSET_DIRECTORY"
#ifndef CHI_VERSION
#define CHI_VERSION "development"
#endif

typedef struct HostConfiguration {
    unsigned long soundCycles;
    char assetDirectory[1024];
} HostConfiguration;

static int SoundCyclesValid(unsigned long cycles)
{
    return cycles==SDLBackend_SoundCpuIbmPc || cycles==SDLBackend_SoundCpu286
        || cycles==SDLBackend_SoundCpu1510 || cycles==SDLBackend_SoundCpuDosBoxNostalgia;
}

static int ParseSoundCycles(const char *value,unsigned long *cycles)
{
    char *end;
    *cycles=strtoul(value,&end,10);
    while(isspace((unsigned char)*end)) ++end;
    return value!=end && *end=='\0' && SoundCyclesValid(*cycles);
}

static int CopyConfigurationValue(char *destination,size_t capacity,const char *value)
{
    size_t length=strlen(value);
    while(length!=0 && isspace((unsigned char)value[length-1])) --length;
    if(length==0 || length>=capacity) return 0;
    memcpy(destination,value,length);
    destination[length]='\0';
    return 1;
}

static int LoadHostConfiguration(const char *basePath,HostConfiguration *settings)
{
    char configPath[1280];
    FILE *config;
    char line[256];
    int lineNumber=0;
    {
        int written=snprintf(configPath,sizeof configPath,"%s%s",basePath,CHI_CONFIG_FILE);
        if(written<0 || (size_t)written>=sizeof configPath) return 0;
    }
#ifdef _MSC_VER
    if(fopen_s(&config,configPath,"r")!=0) config=NULL;
#else
    config=fopen(configPath,"r");
#endif
    if(!config) return 1; /* Older installations retain built-in defaults. */
    while(fgets(line,sizeof line,config)) {
        char *key=line;
        char *value;
        char *end;
        ++lineNumber;
        while(isspace((unsigned char)*key)) ++key;
        if(*key=='#' || *key==';' || *key=='\0') continue;
        value=strchr(key,'=');
        if(!value) break;
        end=value;
        while(end>key && isspace((unsigned char)end[-1])) --end;
        *end='\0';
        ++value;
        while(isspace((unsigned char)*value)) ++value;
        if(strcmp(key,"sound_cycles")==0) {
            if(!ParseSoundCycles(value,&settings->soundCycles)) break;
        } else if(strcmp(key,"asset_directory")==0) {
            if(!CopyConfigurationValue(settings->assetDirectory,
                    sizeof settings->assetDirectory,value)) break;
        } else break;
    }
    if(!feof(config)) {
        fprintf(stderr,"Invalid %s line %d (expected asset_directory=<path> or sound_cycles=240|750|1510|3000).\n",
            configPath,lineNumber);
        fclose(config);
        return 0;
    }
    fclose(config);
    return 1;
}

static int HostSetCurrentDirectory(const char *path)
{
#ifdef _WIN32
    return _chdir(path)==0;
#else
    return chdir(path)==0;
#endif
}

/* Platform entry only: original0D27:0044 owns game setup and startup order.
 * Run from a local original-game asset directory. No replacement gameplay,
 * synthetic initial party or embedded external files are introduced here. */
int main(int argc,char **argv)
{
    HostConfiguration settings={SDLBackend_SoundCpuIbmPc,"."};
    const char *basePath;
    const char *environmentAssetDirectory;
    if(argc==2 && strcmp(argv[1],"--version")==0) {
        printf("CrescentHawksInception %s\n",CHI_VERSION);
        return 0;
    }
    basePath=SDL_GetBasePath();
    if(!basePath || !LoadHostConfiguration(basePath,&settings)) return 2;
    if(argc==3 && strcmp(argv[1],"--sound-cycles")==0) {
        if(!ParseSoundCycles(argv[2],&settings.soundCycles)) {
            fputs("Sound cycles must be 240, 750, 1510 or 3000 cycles/ms.\n",stderr);
            return 2;
        }
    } else if(argc!=1) {
        fputs("Usage: CrescentHawksInception [--version] [--sound-cycles 240|750|1510|3000]\n",stderr);
        return 2;
    }
    environmentAssetDirectory=SDL_getenv(CHI_ASSET_DIRECTORY_ENV);
    if(environmentAssetDirectory && *environmentAssetDirectory) {
        if(!CopyConfigurationValue(settings.assetDirectory,
                sizeof settings.assetDirectory,environmentAssetDirectory)) {
            fprintf(stderr,"Invalid %s value.\n",CHI_ASSET_DIRECTORY_ENV);
            return 2;
        }
    } else if(settings.assetDirectory[0]!='/' && settings.assetDirectory[0]!='\\'
        && !(isalpha((unsigned char)settings.assetDirectory[0])
            && settings.assetDirectory[1]==':')) {
        char relativeAssetDirectory[1024];
        int written=snprintf(relativeAssetDirectory,sizeof relativeAssetDirectory,"%s%s",
            basePath,settings.assetDirectory);
        if(written<0 || (size_t)written>=sizeof relativeAssetDirectory) {
            fputs("Configured asset directory is too long.\n",stderr);
            return 2;
        }
        memcpy(settings.assetDirectory,relativeAssetDirectory,
            strlen(relativeAssetDirectory)+1);
    }
    if(!HostSetCurrentDirectory(settings.assetDirectory)) {
        fprintf(stderr,"Unable to use original asset directory '%s' (host error %d).\n",
            settings.assetDirectory,errno);
        return 2;
    }
    SDLBackend_SetSoundCpuCyclesPerMillisecond((uint32_t)settings.soundCycles);
    if (!SDLBackend_Open()) {
        fputs("Unable to open the SDL video/audio backend.\n",stderr);
        return 1;
    }
    (void)SDLBackend_RunApplication(Setup_Game);
    SDLBackend_Close();
    return 0;
}
