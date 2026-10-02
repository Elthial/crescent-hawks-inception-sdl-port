#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void){
    if(AnimationFrameWorkspace!=GraphicsFileWorkspace || AnimationFileData!=GraphicsFileWorkspace+AnmFrameWorkspaceBytes){
        fprintf(stderr,"Native scene/workspace addresses do not alias\n");return EXIT_FAILURE;
    }
    memset(GraphicsFileWorkspace,0xA5,GraphicsWorkspaceBytes);
    for(unsigned offset=0;offset<sizeof SceneAnimation.bytes;++offset){
        SceneAnimation.bytes[offset]=(uint8_t)offset;
        if(GraphicsFileWorkspace[offset]!=(uint8_t)offset)return EXIT_FAILURE;
        GraphicsFileWorkspace[offset]=(uint8_t)(offset+1);
        if(SceneAnimation.bytes[offset]!=(uint8_t)(offset+1))return EXIT_FAILURE;
    }
    for(unsigned offset=sizeof SceneAnimation.bytes;offset<GraphicsWorkspaceBytes;++offset)
        if(GraphicsFileWorkspace[offset]!=0xA5)return EXIT_FAILURE;
    puts("Native scene/frame/file/graphics storage aliases match every byte.");return EXIT_SUCCESS;
}
