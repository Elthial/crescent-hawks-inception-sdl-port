// Sol:Reconstructed startup workflow from matching0D27 ASM; EGA-only preservation view.
// Sol:Fresh systematic comparison: docs/phase4/BTECH_0D27_SYSTEMATIC_ASM_AUDIT.md.
#include "BTECH.h"
// Sol:Original0213 console character output is a replaceable presentation binding, NOT DOS implementation.
void Platform_Write_Startup_Character_0213(signed short Character);
// Sol: Summary: Print a startup message.
// Sol: Outline: Read a FAR string byte by byte until NUL and pass each character to the platform output binding.
void Startup_Print_Nul_String_000A(unsigned char far *Text)
{
	unsigned short StringIndex = 0;
	unsigned char far *SegmentBase = (unsigned char far *)((unsigned long)Text & 0xFFFF0000UL);
	unsigned short StartOffset = (unsigned short)(unsigned long)Text;
	while (SegmentBase[(unsigned short)(StartOffset + StringIndex)] != 0)
		Platform_Write_Startup_Character_0213((signed char)SegmentBase[(unsigned short)(StartOffset + StringIndex++)]); // Sol:001D CBW; WORD cursor wraps.
}

// Sol:0410..082C: capture ALL376 contiguous IDs; X/width are8-pixel cells, Y/height scanlines.
// Sol: Summary: Build the game's 376 sprite snapshots.
// Sol: Outline: Capture fixed rectangles from the mech artwork and store each allocated sprite in the pointer table.
void Capture_Startup_Sprite_Table_0410()
{
	unsigned short SpriteIndex;
	unsigned short SourceRow;
	for (SpriteIndex = 0; SpriteIndex < 12; ++SpriteIndex)
		Capture_Combat_Sprite_070A(SpriteIndex, 3 * SpriteIndex, 0, 3, 24);
	for (SpriteIndex = 12; SpriteIndex < 16; ++SpriteIndex)
		Capture_Combat_Sprite_070A(SpriteIndex, 3 * SpriteIndex - 36, 24, 3, 24);
	for (SpriteIndex = 16; SpriteIndex < 36; ++SpriteIndex)
		Capture_Combat_Sprite_070A(SpriteIndex, SpriteIndex - 4, 24, 1, 8);
	for (SourceRow = 9; SourceRow < 16; ++SourceRow)
		for (SpriteIndex = 12; SpriteIndex < 24; ++SpriteIndex)
			Capture_Combat_Sprite_070A(12 * SourceRow + SpriteIndex - 84, SpriteIndex, SourceRow << 3, 1, 8);
	Capture_Combat_Sprite_070A(120, 0, 96, 3, 24);
	Capture_Combat_Sprite_070A(121, 3, 96, 3, 24);
	Capture_Combat_Sprite_070A(122, 9, 120, 3, 24);
	Capture_Combat_Sprite_070A(123, 6, 120, 3, 24);
	Capture_Combat_Sprite_070A(124, 8, 160, 2, 14);
	Capture_Combat_Sprite_070A(125, 10, 160, 2, 14);
	Capture_Combat_Sprite_070A(126, 0, 160, 1, 8);
	Capture_Combat_Sprite_070A(127, 1, 160, 1, 8);
	Capture_Combat_Sprite_070A(128, 8, 144, 2, 16);
	Capture_Combat_Sprite_070A(129, 10, 144, 2, 16);
	for (SourceRow = 18; SourceRow < 22; ++SourceRow)
		for (SpriteIndex = 4; SpriteIndex < 8; ++SpriteIndex)
			Capture_Combat_Sprite_070A(4 * SourceRow + SpriteIndex + 54, SpriteIndex, SourceRow << 3, 1, 8);
	for (SpriteIndex = 0; SpriteIndex < 12; ++SpriteIndex)
		Capture_Combat_Sprite_070A(SpriteIndex + 146, 3 * SpriteIndex, 48, 3, 24);
	for (SpriteIndex = 0; SpriteIndex < 4; ++SpriteIndex)
		Capture_Combat_Sprite_070A(SpriteIndex + 158, 3 * SpriteIndex, 72, 3, 24);
	Capture_Combat_Sprite_070A(162, 6, 96, 3, 24);
	Capture_Combat_Sprite_070A(163, 9, 96, 3, 24);
	Capture_Combat_Sprite_070A(164, 0, 120, 3, 24);
	Capture_Combat_Sprite_070A(165, 3, 120, 3, 24);
	for (SpriteIndex = 166; SpriteIndex < 186; ++SpriteIndex)
		Capture_Combat_Sprite_070A(SpriteIndex, SpriteIndex - 154, 32, 1, 8);
	for (SourceRow = 9; SourceRow < 16; ++SourceRow)
		for (SpriteIndex = 24; SpriteIndex < 36; ++SpriteIndex)
			Capture_Combat_Sprite_070A(12 * SourceRow + SpriteIndex + 54, SpriteIndex, SourceRow << 3, 1, 8);
	for (SpriteIndex = 266; SpriteIndex < 286; ++SpriteIndex)
		Capture_Combat_Sprite_070A(SpriteIndex + 4, SpriteIndex - 254, 40, 1, 8);
	for (SourceRow = 16; SourceRow < 23; ++SourceRow)
		for (SpriteIndex = 12; SpriteIndex < 24; ++SpriteIndex)
			Capture_Combat_Sprite_070A(12 * SourceRow + SpriteIndex + 86, SpriteIndex, SourceRow << 3, 1, 8);
	Capture_Combat_Sprite_070A(374, 0, 144, 2, 11);
	Capture_Combat_Sprite_070A(375, 2, 144, 2, 11);
}

// Sol:Original main0D27:0044, reconstructed with only the owner-retained EGA graphics path.
// Sol:Skipped adapter-choice prompt/native non-EGA bodies are documented, not restored.
// Sol: Summary: Initialise assets and enter the game.
// Sol: Outline: Choose the asset layout, initialise EGA, load title and tiles, capture sprites, and call Start_Game.
void Setup_Game()
{
	// Sol:Audit:014C/0223/02A5/0327 call207F:00D1 natively. It skips all table writes
	// Sol:when246C:B764!=0; EGA setup sets2, so these omitted translation calls are deliberate no-ops.
	Startup_Print_Nul_String_000A((unsigned char far *)"\r\nBattleTech: The Crescent Hawk's Inception\r\n(C) 1988 Infocom, Inc.\r\nVersion 1.03\r\n"); // Sol:native startup banner. // Sol: exact EXE text 3EDB:0CD6; readable text restored
	seg3EDB->GraphicsAdapter = EGA_VGA; // Sol:EGA-only preservation restriction; native accepts ASCII1..4 then subtracts'1'.
	Startup_Print_Nul_String_000A((unsigned char far *)"\r\nHow many disk drives?\r\n\n1. One\r\n2. Two\r\n3. Hard disk C:\r\n? "); // Sol:native drive-layout question, not gameplay state. // Sol: exact EXE text 3EDB:0D92; readable text restored
	seg3092->HasHardDisk_Bool = FALSE;
	signed short DriveChoice;
	do { DriveChoice = Keyboard_Get_ASCII_Hex_Input(); } while (DriveChoice < '1' || DriveChoice > '3');
	seg3092->SecondFloppyDriveAvailable = DriveChoice - '1';
	if (DriveChoice == '3')
	{
		seg3092->SecondFloppyDriveAvailable = FALSE;
		seg3092->HasHardDisk_Bool = TRUE;
		Startup_Print_Nul_String_000A((unsigned char far *)"\r\nAll files from both BattleTech disks should be in the current directory."); // Sol: exact EXE text 3EDB:0E2C; readable text restored
		Startup_Print_Nul_String_000A((unsigned char far *)"\r\nYour Saved Games will be saved onto your hard disk."); // Sol: exact EXE text 3EDB:0E78; readable text restored
		Keyboard_Get_ASCII_Hex_Input();
	}
	if (seg3092->SecondFloppyDriveAvailable != FALSE)
	{
		Startup_Print_Nul_String_000A((unsigned char far *)"\r\nAlways keep the Game disk in A:, and your copy of Disk 2 in drive B: to\r\nsave games onto."); // Sol: exact EXE text 3EDB:0DD0; readable text restored
		Keyboard_Get_ASCII_Hex_Input();
	}
	Initialize_Graphics_Runtime_0C8F(); // Sol:platform bootstrap boundary: no new BIOS/DOS implementation.
	Select_Game_Disk_And_Drive_28CC(2);
	Load_File_To_Memory("INFOCOM.CMP", (unsigned char far *)&seg3092->Ptr_File_To_Memory_Block);
	Decompress_File_Into_Memory((unsigned char far *)&seg3092->Ptr_File_To_Memory_Block, (unsigned char far *)&seg246C->Ptr_DecompressedFile_Memory_Block);
	DrawCall_Image_To_VGA_Memory((unsigned char far *)&seg246C->Ptr_DecompressedFile_Memory_Block, VGA_MEMORYADDRESS); // Sol:A800 staged image.
	Set_Palette_registers(seg2FE8->DefaultEgaPalette);
	Draw_GraphicsFile_In_Memory((unsigned char far *)&seg246C->Ptr_DecompressedFile_Memory_Block, 0, 0, 40, 200);
	Drain_Pending_Keyboard_Input_2A2B();
	unsigned short SplashRetracesRemaining = 700; // Sol:2BCh, interruptible retrace countdown; not milliseconds.
	while (SplashRetracesRemaining-- != 0)
	{
		Wait_For_N_Vertical_Retraces(1);
		if (Pending_Input() != FALSE) SplashRetracesRemaining = 0;
	}
	Drain_Pending_Keyboard_Input_2A2B();
	Load_And_Draw_BTTITLE_CMP();
	Load_File_To_Memory("BTBORDER.CMP", (unsigned char far *)&seg3092->Ptr_File_To_Memory_Block);
	Decompress_File_Into_Memory((unsigned char far *)&seg3092->Ptr_File_To_Memory_Block, (unsigned char far *)&seg246C->Ptr_DecompressedFile_Memory_Block);
	DrawCall_Image_To_VGA_Memory((unsigned char far *)&seg246C->Ptr_DecompressedFile_Memory_Block, VGA_MEMORYADDRESS);
	seg3092->BTBORDER_TileSet = Create_TileSet_Array((unsigned char far *)&seg246C->Ptr_DecompressedFile_Memory_Block, 0, 0, 8); // Sol:4066/4068 FAR return.
	Load_File_To_Memory("TINYLAND.CMP", (unsigned char far *)&seg3092->Ptr_File_To_Memory_Block);
	Decompress_File_Into_Memory((unsigned char far *)&seg3092->Ptr_File_To_Memory_Block, (unsigned char far *)&seg246C->Ptr_DecompressedFile_Memory_Block);
	DrawCall_Image_To_VGA_Memory((unsigned char far *)&seg246C->Ptr_DecompressedFile_Memory_Block, VGA_MEMORYADDRESS);
	seg3092->TINYLAND_TileSet = Create_TileSet_Array((unsigned char far *)&seg246C->Ptr_DecompressedFile_Memory_Block, 0, 0, 66); // Sol:4588/458A FAR return.
	Load_File_To_Memory("MECHSHAP.CMP", (unsigned char far *)&seg3092->Ptr_File_To_Memory_Block);
	Decompress_File_Into_Memory((unsigned char far *)&seg3092->Ptr_File_To_Memory_Block, (unsigned char far *)&seg246C->Ptr_DecompressedFile_Memory_Block);
	VGA_Inline_ASM_Loop((unsigned char far *)&seg246C->Ptr_DecompressedFile_Memory_Block, (unsigned char far *)&seg246C->Ptr_DecompressedFile_Memory_Block, 16000); // Sol:03F5 in-place packed->interleaved planar conversion BEFORE captures.
	Capture_Startup_Sprite_Table_0410();
	Start_Game(); // Sol:0834 enters existing title/attractor/game controller0800:50C8.
	Restore_BIOS_Text_Mode_0D12(); // Sol:0839 release presentation backend in a portable port; native BIOS text mode2.
}
