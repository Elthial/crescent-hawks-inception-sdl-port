#include "game.h"
#include "dos.h"

/* Sol: Original0D27:000A. DOS character service is a platform boundary;
 * signed BYTE-to-WORD character conversion and forward string order remain. */
void Startup_Print_Nul_String(uint8_t *text)
{
    uint16_t index=0;
    while (text[index] != 0)
        Platform_Write_Startup_Character((int16_t)(int8_t)text[index++]);
}

enum { SplashDisplayRetraces=700, BorderStartupTileCount=8, TinylandStartupTileCount=66 };

/* Sol: Original0D27:0044 main, retained owner EGA-only restriction. Full raw
 * startup/capture blocks checked. Deleted adapter prompt/bodies and EGA-no-op
 * 00D1 translations stay absent. Native capture schedule remains inline, not
 * the annotation-only extracted0410 helper. Atlas coordinates/ID ranges below
 * are literal original artwork layout, not new gameplay rules or descriptors.
 * Startup services and Start_Game remain original dependencies, no substitutes. */
void Setup_Game(void)
{
	Startup_Print_Nul_String((uint8_t *)"\r\nBattleTech: The Crescent Hawk's Inception\r\n(C) 1988 Infocom, Inc.\r\nVersion 1.03\r\n");
	/* SDL preservation adaptation: original removable-drive discovery is not a
	 * meaningful host choice.  Keep the recovered DOS interaction below for
	 * reference, but behave exactly as its option3 (installed on hard disk).
	 * Host files remain relative to the executable's working directory. */
#if 0
	Startup_Print_Nul_String((uint8_t *)"\r\nHow many disk drives?\r\n\n1. One\r\n2. Two\r\n3. Hard disk C:\r\n? ");
	HasHardDisk = FALSE;
	int16_t driveChoice;
	do { driveChoice = Keyboard_Get_ASCII_Hex_Input(); } while (driveChoice < '1' || driveChoice > '3');
	SecondFloppyDriveAvailable = driveChoice - '1';
	if (driveChoice == '3')
	{
		SecondFloppyDriveAvailable = FALSE;
		HasHardDisk = TRUE;
		Startup_Print_Nul_String((uint8_t *)"\r\nAll files from both BattleTech disks should be in the current directory.");
		Startup_Print_Nul_String((uint8_t *)"\r\nYour Saved Games will be saved onto your hard disk.");
		Keyboard_Get_ASCII_Hex_Input();
	}
	if (SecondFloppyDriveAvailable != FALSE)
	{
		Startup_Print_Nul_String((uint8_t *)"\r\nAlways keep the Game disk in A:, and your copy of Disk 2 in drive B: to\r\nsave games onto.");
		Keyboard_Get_ASCII_Hex_Input();
	}
#endif
	SecondFloppyDriveAvailable = FALSE;
	HasHardDisk = TRUE;
	GraphicsAdapter=GraphicsAdapter_Ega; /* maintained original main's EGA-only selection */
	Initialize_Graphics_Runtime();
	Select_Game_Disk_And_Drive(GameDisk_Second);
	Load_File_To_Memory((const uint8_t *)"INFOCOM.CMP", GraphicsSceneWorkspace);
	Decompress_File_Into_Memory(GraphicsSceneWorkspace, GraphicsFileWorkspace);
	DrawCall_Image_To_VGA_Memory(GraphicsFileWorkspace, EgaSceneStagingSegment);
	Set_Palette_registers(DefaultEgaPalette);
	Draw_GraphicsFile_In_Memory(GraphicsFileWorkspace, 0, 0, EgaFramebufferRowBytes, EgaScreenHeight);
	Drain_Pending_Keyboard_Input();
	uint16_t splashRetracesRemaining = SplashDisplayRetraces;
	while (splashRetracesRemaining-- != 0)
	{
		Wait_For_N_Vertical_Retraces(1);
		if (Pending_Input() != FALSE) splashRetracesRemaining = 0;
	}
	Drain_Pending_Keyboard_Input();
	Load_And_Draw_BTTITLE_CMP();
	Load_File_To_Memory((const uint8_t *)"BTBORDER.CMP", GraphicsSceneWorkspace);
	Decompress_File_Into_Memory(GraphicsSceneWorkspace, GraphicsFileWorkspace);
	DrawCall_Image_To_VGA_Memory(GraphicsFileWorkspace, EgaSceneStagingSegment);
	BorderTileset = Create_TileSet_Array(GraphicsFileWorkspace, 0, 0, BorderStartupTileCount);
	Load_File_To_Memory((const uint8_t *)"TINYLAND.CMP", GraphicsSceneWorkspace);
	Decompress_File_Into_Memory(GraphicsSceneWorkspace, GraphicsFileWorkspace);
	DrawCall_Image_To_VGA_Memory(GraphicsFileWorkspace, EgaSceneStagingSegment);
	TinylandTileset = Create_TileSet_Array(GraphicsFileWorkspace, 0, 0, TinylandStartupTileCount);
	Load_File_To_Memory((const uint8_t *)"MECHSHAP.CMP", GraphicsSceneWorkspace);
	Decompress_File_Into_Memory(GraphicsSceneWorkspace, GraphicsFileWorkspace);
	VGA_Inline_ASM_Loop(GraphicsFileWorkspace, GraphicsFileWorkspace, PackedGraphicsOutputBytes/2);
	/* Native040B..0833 schedule, inline: not a new helper ABI. */
	uint16_t spriteIndex;
	uint16_t sourceRow;
	for (spriteIndex = 0; spriteIndex < 12; ++spriteIndex)
		Capture_Combat_Sprite(spriteIndex, 3 * spriteIndex, 0, 3, 24);
	for (spriteIndex = 12; spriteIndex < 16; ++spriteIndex)
		Capture_Combat_Sprite(spriteIndex, 3 * spriteIndex - 36, 24, 3, 24);
	for (spriteIndex = 16; spriteIndex < 36; ++spriteIndex)
		Capture_Combat_Sprite(spriteIndex, spriteIndex - 4, 24, 1, 8);
	for (sourceRow = 9; sourceRow < 16; ++sourceRow)
		for (spriteIndex = 12; spriteIndex < 24; ++spriteIndex)
			Capture_Combat_Sprite(12 * sourceRow + spriteIndex - 84, spriteIndex, sourceRow << 3, 1, 8);
	Capture_Combat_Sprite(120, 0, 96, 3, 24);
	Capture_Combat_Sprite(121, 3, 96, 3, 24);
	Capture_Combat_Sprite(122, 9, 120, 3, 24);
	Capture_Combat_Sprite(123, 6, 120, 3, 24);
	Capture_Combat_Sprite(124, 8, 160, 2, 14);
	Capture_Combat_Sprite(125, 10, 160, 2, 14);
	Capture_Combat_Sprite(126, 0, 160, 1, 8);
	Capture_Combat_Sprite(127, 1, 160, 1, 8);
	Capture_Combat_Sprite(128, 8, 144, 2, 16);
	Capture_Combat_Sprite(129, 10, 144, 2, 16);
	for (sourceRow = 18; sourceRow < 22; ++sourceRow)
		for (spriteIndex = 4; spriteIndex < 8; ++spriteIndex)
			Capture_Combat_Sprite(4 * sourceRow + spriteIndex + 54, spriteIndex, sourceRow << 3, 1, 8);
	for (spriteIndex = 0; spriteIndex < 12; ++spriteIndex)
		Capture_Combat_Sprite(spriteIndex + 146, 3 * spriteIndex, 48, 3, 24);
	for (spriteIndex = 0; spriteIndex < 4; ++spriteIndex)
		Capture_Combat_Sprite(spriteIndex + 158, 3 * spriteIndex, 72, 3, 24);
	Capture_Combat_Sprite(162, 6, 96, 3, 24);
	Capture_Combat_Sprite(163, 9, 96, 3, 24);
	Capture_Combat_Sprite(164, 0, 120, 3, 24);
	Capture_Combat_Sprite(165, 3, 120, 3, 24);
	for (spriteIndex = 166; spriteIndex < 186; ++spriteIndex)
		Capture_Combat_Sprite(spriteIndex, spriteIndex - 154, 32, 1, 8);
	for (sourceRow = 9; sourceRow < 16; ++sourceRow)
		for (spriteIndex = 24; spriteIndex < 36; ++spriteIndex)
			Capture_Combat_Sprite(12 * sourceRow + spriteIndex + 54, spriteIndex, sourceRow << 3, 1, 8);
	for (spriteIndex = 266; spriteIndex < 286; ++spriteIndex)
		Capture_Combat_Sprite(spriteIndex + 4, spriteIndex - 254, 40, 1, 8);
	for (sourceRow = 16; sourceRow < 23; ++sourceRow)
		for (spriteIndex = 12; spriteIndex < 24; ++spriteIndex)
			Capture_Combat_Sprite(12 * sourceRow + spriteIndex + 86, spriteIndex, sourceRow << 3, 1, 8);
	Capture_Combat_Sprite(374, 0, 144, 2, 11);
	Capture_Combat_Sprite(375, 2, 144, 2, 11);
	Start_Game();
	Restore_BIOS_Text_Mode();
}
