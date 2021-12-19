#define XA_LENGTH(x) (((u64)(x) * 75) / 100 * IO_SECT_SIZE) //Centiseconds to sectors in bytes (w)

typedef struct
{
	XA_File file;
	u32 length;
} XA_TrackDef;

static const XA_TrackDef xa_tracks[] = {
	//MENU.XA
	{XA_Menu, XA_LENGTH(11400)}, //XA_GettinFreaky
	{XA_Menu, XA_LENGTH(2100)},  //XA_GameOver
	//WEEK1A.XA
	{XA_Week1A, XA_LENGTH(8300)}, //XA_Bopeebo
	{XA_Week1A, XA_LENGTH(7700)}, //XA_Fresh
	//WEEK1B.XA
	{XA_Week1B, XA_LENGTH(8800)}, //XA_Dadbattle
	{XA_Week1B, XA_LENGTH(9400)}, //XA_Tutorial
	//WEEK1C.XA
	{XA_Week1C, XA_LENGTH(12900)}, //XA_Serpent
	//WEEK2A.XA
	{XA_Week2A, XA_LENGTH(10200)}, //XA_Spookeez
	{XA_Week2A, XA_LENGTH(13000)}, //XA_South
	//WEEK2B.XA
	{XA_Week2B, XA_LENGTH(5700)}, //XA_Monster
	{XA_Week2B, XA_LENGTH(11000)}, //XA_Clucked
	//WEEK3A.XA
	{XA_Week3A, XA_LENGTH(8300)},  //XA_Pico
	{XA_Week3A, XA_LENGTH(7600)}, //XA_Philly
	//WEEK3B.XA
	{XA_Week3B, XA_LENGTH(7600)}, //XA_Blammed
	//WEEK4A.XA
	{XA_Week4A, XA_LENGTH(8300)},  //XA_SatinPanties
	{XA_Week4A, XA_LENGTH(7700)}, //XA_High
	//WEEK4B.XA
	{XA_Week4B, XA_LENGTH(8700)}, //XA_MILF
	{XA_Week4B, XA_LENGTH(7600)}, //XA_Test
	//WEEK5A.XA
	{XA_Week5A, XA_LENGTH(9100)}, //XA_Cocoa
	{XA_Week5A, XA_LENGTH(9401)},  //XA_Eggnog
};

static const char *xa_paths[] = {
	"\\MUSIC\\MENU.XA;1",   //XA_Menu
	"\\MUSIC\\WEEK1A.XA;1", //XA_Week1A
	"\\MUSIC\\WEEK1B.XA;1", //XA_Week1B
	"\\MUSIC\\WEEK1C.XA;1", //XA_Week1C
	"\\MUSIC\\WEEK2A.XA;1", //XA_Week2A
	"\\MUSIC\\WEEK2B.XA;1", //XA_Week2B
	"\\MUSIC\\WEEK3A.XA;1", //XA_Week3A
	"\\MUSIC\\WEEK3B.XA;1", //XA_Week3B
	"\\MUSIC\\WEEK4A.XA;1", //XA_Week4A
	"\\MUSIC\\WEEK4B.XA;1", //XA_Week4B
	"\\MUSIC\\WEEK5A.XA;1", //XA_Week5A
	NULL,
};

typedef struct
{
	const char *name;
	boolean vocal;
} XA_Mp3;

static const XA_Mp3 xa_mp3s[] = {
	//MENU.XA
	{"freaky", false},   //XA_GettinFreaky
	{"gameover", false}, //XA_GameOver
	//WEEK1A.XA
	{"bopeebo", true}, //XA_Bopeebo
	{"fresh", true},   //XA_Fresh
	//WEEK1B.XA
	{"dadbattle", true}, //XA_Dadbattle
	{"tutorial", true}, //XA_Tutorial
	//WEEK1C.XA
	{"serpent", true}, //XA_Serpent
	//WEEK2A.XA
	{"spookeez", true}, //XA_Spookeez
	{"south", true},    //XA_South
	//WEEK2B.XA
	{"monster", true}, //XA_Monster
	{"clucked", true}, //XA_Clucked
	//WEEK3A.XA
	{"pico", true},   //XA_Pico
	{"philly", true}, //XA_Philly
	//WEEK3B.XA
	{"blammed", true}, //XA_Blammed
	//WEEK4A.XA
	{"satinpanties", true}, //XA_SatinPanties
	{"high", true},         //XA_High
	//WEEK4B.XA
	{"milf", true}, //XA_MILF
	{"test", true}, //XA_Test
	//WEEK5A.XA
	{"cocoa", true},  //XA_Cocoa
	{"eggnog", true}, //XA_Eggnog
	
	{NULL, false}
};
