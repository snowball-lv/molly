#include <ide.h>
#include <console.h>
#include <io.h>
#include <molly.h>
#include <klib.h>

// Registers:
#define REG_DATA       	0x00
#define REG_ERROR      	0x01
#define REG_FEATURES   	0x01
#define REG_SECCOUNT0  	0x02
#define REG_LBA0       	0x03
#define REG_LBA1       	0x04
#define REG_LBA2       	0x05
#define REG_HDDEVSEL   	0x06
#define REG_COMMAND    	0x07
#define REG_STATUS     	0x07
#define REG_SECCOUNT1  	0x08
#define REG_LBA3       	0x09
#define REG_LBA4       	0x0A
#define REG_LBA5       	0x0B
#define REG_CONTROL    	0x0C
#define REG_ALTSTATUS  	0x0C
#define REG_DEVADDRESS 	0x0D

//Statuses:
#define ATA_SR_BSY     0x80
#define ATA_SR_DRDY    0x40
#define ATA_SR_DF      0x20
#define ATA_SR_DSC     0x10
#define ATA_SR_DRQ     0x08
#define ATA_SR_CORR    0x04
#define ATA_SR_IDX     0x02
#define ATA_SR_ERR     0x01

#define MASTER	0
#define SLAVE	1

//Commands:
#define CMD_READ_PIO          0x20
#define CMD_READ_PIO_EXT      0x24
#define CMD_READ_DMA          0xC8
#define CMD_READ_DMA_EXT      0x25
#define CMD_WRITE_PIO         0x30
#define CMD_WRITE_PIO_EXT     0x34
#define CMD_WRITE_DMA         0xCA
#define CMD_WRITE_DMA_EXT     0x35
#define CMD_CACHE_FLUSH       0xE7
#define CMD_CACHE_FLUSH_EXT   0xEA
#define CMD_PACKET            0xA0
#define CMD_IDENTIFY_PACKET   0xA1
#define CMD_IDENTIFY          0xEC

typedef struct {
	int base;
	int ctrl;
	int bmide;
} Channel;

static Channel channels[2];

static char ide_read(int channel, int reg) {
   
	char ret = 0;
   
   //if (reg > 0x07 && reg < 0x0C)
		//ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
	  
	if (reg < 0x08)
	  ret = in8(channels[channel].base + reg - 0x00);
	else if (reg < 0x0C)
	  ret = in8(channels[channel].base  + reg - 0x06);
	else if (reg < 0x0E)
	  ret = in8(channels[channel].ctrl  + reg - 0x0A);
	else if (reg < 0x16)
	  ret = in8(channels[channel].bmide + reg - 0x0E);
	  
	//if (reg > 0x07 && reg < 0x0C)
		//ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
	  
	return ret;
}

static void ide_write(int channel, int reg, char data) {

	//if (reg > 0x07 && reg < 0x0C)
	//	ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);

	if (reg < 0x08)
		out8(channels[channel].base  + reg - 0x00, data);
	else if (reg < 0x0C)
		out8(channels[channel].base  + reg - 0x06, data);
	else if (reg < 0x0E)
		out8(channels[channel].ctrl  + reg - 0x0A, data);
	else if (reg < 0x16)
		out8(channels[channel].bmide + reg - 0x0E, data);

	//if (reg > 0x07 && reg < 0x0C)
	//	ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}

typedef struct {
  struct {
    uint16_t Reserved1  :1;
    uint16_t Retired3  :1;
    uint16_t ResponseIncomplete  :1;
    uint16_t Retired2  :3;
    uint16_t FixedDevice  :1;
    uint16_t RemovableMedia  :1;
    uint16_t Retired1  :7;
    uint16_t DeviceType  :1;
  } GeneralConfiguration;
  uint16_t NumCylinders;
  uint16_t ReservedWord2;
  uint16_t NumHeads;
  uint16_t Retired1[2];
  uint16_t NumSectorsPerTrack;
  uint16_t VendorUnique1[3];
  uint8_t  SerialNumber[20];
  uint16_t Retired2[2];
  uint16_t Obsolete1;
  uint8_t  FirmwareRevision[8];
  uint8_t  ModelNumber[40];
  uint8_t  MaximumBlockTransfer;
  uint8_t  VendorUnique2;
  uint16_t ReservedWord48;
  struct {
    uint8_t  ReservedByte49;
    uint8_t  DmaSupported  :1;
    uint8_t  LbaSupported  :1;
    uint8_t  IordyDisable  :1;
    uint8_t  IordySupported  :1;
    uint8_t  Reserved1  :1;
    uint8_t  StandybyTimerSupport  :1;
    uint8_t  Reserved2  :2;
    uint16_t ReservedWord50;
  } Capabilities;
  uint16_t ObsoleteWords51[2];
  uint16_t TranslationFieldsValid  :3;
  uint16_t Reserved3  :13;
  uint16_t NumberOfCurrentCylinders;
  uint16_t NumberOfCurrentHeads;
  uint16_t CurrentSectorsPerTrack;
  uint32_t  CurrentSectorCapacity;
  uint8_t  CurrentMultiSectorSetting;
  uint8_t  MultiSectorSettingValid  :1;
  uint8_t  ReservedByte59  :7;
  uint32_t  UserAddressableSectors;
  uint16_t ObsoleteWord62;
  uint16_t MultiWordDMASupport  :8;
  uint16_t MultiWordDMAActive  :8;
  uint16_t AdvancedPIOModes  :8;
  uint16_t ReservedByte64  :8;
  uint16_t MinimumMWXferCycleTime;
  uint16_t RecommendedMWXferCycleTime;
  uint16_t MinimumPIOCycleTime;
  uint16_t MinimumPIOCycleTimeIORDY;
  uint16_t ReservedWords69[6];
  uint16_t QueueDepth  :5;
  uint16_t ReservedWord75  :11;
  uint16_t ReservedWords76[4];
  uint16_t MajorRevision;
  uint16_t MinorRevision;
  struct {
    uint16_t SmartCommands  :1;
    uint16_t SecurityMode  :1;
    uint16_t RemovableMediaFeature  :1;
    uint16_t PowerManagement  :1;
    uint16_t Reserved1  :1;
    uint16_t WriteCache  :1;
    uint16_t LookAhead  :1;
    uint16_t ReleaseInterrupt  :1;
    uint16_t ServiceInterrupt  :1;
    uint16_t DeviceReset  :1;
    uint16_t HostProtectedArea  :1;
    uint16_t Obsolete1  :1;
    uint16_t WriteBuffer  :1;
    uint16_t ReadBuffer  :1;
    uint16_t Nop  :1;
    uint16_t Obsolete2  :1;
    uint16_t DownloadMicrocode  :1;
    uint16_t DmaQueued  :1;
    uint16_t Cfa  :1;
    uint16_t AdvancedPm  :1;
    uint16_t Msn  :1;
    uint16_t PowerUpInStandby  :1;
    uint16_t ManualPowerUp  :1;
    uint16_t Reserved2  :1;
    uint16_t SetMax  :1;
    uint16_t Acoustics  :1;
    uint16_t BigLba  :1;
    uint16_t DeviceConfigOverlay  :1;
    uint16_t FlushCache  :1;
    uint16_t FlushCacheExt  :1;
    uint16_t Resrved3  :2;
    uint16_t SmartErrorLog  :1;
    uint16_t SmartSelfTest  :1;
    uint16_t MediaSerialNumber  :1;
    uint16_t MediaCardPassThrough  :1;
    uint16_t StreamingFeature  :1;
    uint16_t GpLogging  :1;
    uint16_t WriteFua  :1;
    uint16_t WriteQueuedFua  :1;
    uint16_t WWN64Bit  :1;
    uint16_t URGReadStream  :1;
    uint16_t URGWriteStream  :1;
    uint16_t ReservedForTechReport  :2;
    uint16_t IdleWithUnloadFeature  :1;
    uint16_t Reserved4  :2;
  } CommandSetSupport;
  struct {
    uint16_t SmartCommands  :1;
    uint16_t SecurityMode  :1;
    uint16_t RemovableMediaFeature  :1;
    uint16_t PowerManagement  :1;
    uint16_t Reserved1  :1;
    uint16_t WriteCache  :1;
    uint16_t LookAhead  :1;
    uint16_t ReleaseInterrupt  :1;
    uint16_t ServiceInterrupt  :1;
    uint16_t DeviceReset  :1;
    uint16_t HostProtectedArea  :1;
    uint16_t Obsolete1  :1;
    uint16_t WriteBuffer  :1;
    uint16_t ReadBuffer  :1;
    uint16_t Nop  :1;
    uint16_t Obsolete2  :1;
    uint16_t DownloadMicrocode  :1;
    uint16_t DmaQueued  :1;
    uint16_t Cfa  :1;
    uint16_t AdvancedPm  :1;
    uint16_t Msn  :1;
    uint16_t PowerUpInStandby  :1;
    uint16_t ManualPowerUp  :1;
    uint16_t Reserved2  :1;
    uint16_t SetMax  :1;
    uint16_t Acoustics  :1;
    uint16_t BigLba  :1;
    uint16_t DeviceConfigOverlay  :1;
    uint16_t FlushCache  :1;
    uint16_t FlushCacheExt  :1;
    uint16_t Resrved3  :2;
    uint16_t SmartErrorLog  :1;
    uint16_t SmartSelfTest  :1;
    uint16_t MediaSerialNumber  :1;
    uint16_t MediaCardPassThrough  :1;
    uint16_t StreamingFeature  :1;
    uint16_t GpLogging  :1;
    uint16_t WriteFua  :1;
    uint16_t WriteQueuedFua  :1;
    uint16_t WWN64Bit  :1;
    uint16_t URGReadStream  :1;
    uint16_t URGWriteStream  :1;
    uint16_t ReservedForTechReport  :2;
    uint16_t IdleWithUnloadFeature  :1;
    uint16_t Reserved4  :2;
  } CommandSetActive;
  uint16_t UltraDMASupport  :8;
  uint16_t UltraDMAActive  :8;
  uint16_t ReservedWord89[4];
  uint16_t HardwareResetResult;
  uint16_t CurrentAcousticValue  :8;
  uint16_t RecommendedAcousticValue  :8;
  uint16_t ReservedWord95[5];
  uint32_t  Max48BitLBA[2];
  uint16_t StreamingTransferTime;
  uint16_t ReservedWord105;
  struct {
    uint16_t LogicalSectorsPerPhysicalSector  :4;
    uint16_t Reserved0  :8;
    uint16_t LogicalSectorLongerThan256Words  :1;
    uint16_t MultipleLogicalSectorsPerPhysicalSector  :1;
    uint16_t Reserved1  :2;
  } PhysicalLogicalSectorSize;
  uint16_t InterSeekDelay;
  uint16_t WorldWideName[4];
  uint16_t ReservedForWorldWideName128[4];
  uint16_t ReservedForTlcTechnicalReport;
  uint16_t WordsPerLogicalSector[2];
  struct {
    uint16_t ReservedForDrqTechnicalReport  :1;
    uint16_t WriteReadVerifySupported  :1;
    uint16_t Reserved01  :11;
    uint16_t Reserved1  :2;
  } CommandSetSupportExt;
  struct {
    uint16_t ReservedForDrqTechnicalReport  :1;
    uint16_t WriteReadVerifyEnabled  :1;
    uint16_t Reserved01  :11;
    uint16_t Reserved1  :2;
  } CommandSetActiveExt;
  uint16_t ReservedForExpandedSupportandActive[6];
  uint16_t MsnSupport  :2;
  uint16_t ReservedWord1274  :14;
  struct {
    uint16_t SecuritySupported  :1;
    uint16_t SecurityEnabled  :1;
    uint16_t SecurityLocked  :1;
    uint16_t SecurityFrozen  :1;
    uint16_t SecurityCountExpired  :1;
    uint16_t EnhancedSecurityEraseSupported  :1;
    uint16_t Reserved0  :2;
    uint16_t SecurityLevel  :1;
    uint16_t Reserved1  :7;
  } SecurityStatus;
  uint16_t ReservedWord129[31];
  struct {
    uint16_t MaximumCurrentInMA2  :12;
    uint16_t CfaPowerMode1Disabled  :1;
    uint16_t CfaPowerMode1Required  :1;
    uint16_t Reserved0  :1;
    uint16_t Word160Supported  :1;
  } CfaPowerModel;
  uint16_t ReservedForCfaWord161[8];
  struct {
    uint16_t SupportsTrim  :1;
    uint16_t Reserved0  :15;
  } DataSetManagementFeature;
  uint16_t ReservedForCfaWord170[6];
  uint16_t CurrentMediaSerialNumber[30];
  uint16_t ReservedWord206;
  uint16_t ReservedWord207[2];
  struct {
    uint16_t AlignmentOfLogicalWithinPhysical  :14;
    uint16_t Word209Supported  :1;
    uint16_t Reserved0  :1;
  } BlockAlignment;
  uint16_t WriteReadVerifySectorCountMode3Only[2];
  uint16_t WriteReadVerifySectorCountMode2Only[2];
  struct {
    uint16_t NVCachePowerModeEnabled  :1;
    uint16_t Reserved0  :3;
    uint16_t NVCacheFeatureSetEnabled  :1;
    uint16_t Reserved1  :3;
    uint16_t NVCachePowerModeVersion  :4;
    uint16_t NVCacheFeatureSetVersion  :4;
  } NVCacheCapabilities;
  uint16_t NVCacheSizeLSW;
  uint16_t NVCacheSizeMSW;
  uint16_t NominalMediaRotationRate;
  uint16_t ReservedWord218;
  struct {
    uint8_t NVCacheEstimatedTimeToSpinUpInSeconds;
    uint8_t Reserved;
  } NVCacheOptions;
  uint16_t ReservedWord220[35];
  uint16_t Signature  :8;
  uint16_t CheckSum  :8;
} __attribute__((packed)) IDENT;

_Static_assert(sizeof(IDENT) == 512, "IDENT size not 512");

static IDENT ispace;

static void read_sec(int channel, int64_t lba) {

	char vals[6] = {
		(lba >> 0) 	& 0xff,
		(lba >> 8) 	& 0xff,
		(lba >> 16)	& 0xff,
		(lba >> 24)	& 0xff,
		(lba >> 32)	& 0xff,
		(lba >> 40)	& 0xff,
	};
	
	ide_write(channel, REG_SECCOUNT1, 	0); 		stall(1);
	ide_write(channel, REG_LBA3, 		vals[3]); 	stall(1);
	ide_write(channel, REG_LBA4, 		vals[4]); 	stall(1);
	ide_write(channel, REG_LBA5, 		vals[5]); 	stall(1);
	ide_write(channel, REG_SECCOUNT0, 	1); 		stall(1);
	ide_write(channel, REG_LBA0, 		vals[0]); 	stall(1);
	ide_write(channel, REG_LBA1, 		vals[1]); 	stall(1);
	ide_write(channel, REG_LBA2, 		vals[2]); 	stall(1);
	
	ide_write(channel, REG_COMMAND,	CMD_READ_PIO_EXT);
	stall(1);
	
	while (ide_read(channel, REG_STATUS) & ATA_SR_BSY);
	  
	while (1) {
		char sr = ide_read(channel, REG_STATUS);
		if (sr & ATA_SR_DRQ)
			break;
		else if (sr & ATA_SR_ERR)
			panic("***ide error");
	}
	
	for (int i = 0; i < 256; i++) {
		int16_t data = in16(0x1F0);
		kprintf("%c%c", (data >> 0) & 0xff, (data >> 8) & 0xff);
	}
	kprintfln("");
	
}

static void detect_device(int channel, int device) {
	
	ide_write(channel, REG_HDDEVSEL, 0xa0 | 0x40 | (device << 4));
	stall(1);
	
	ide_write(channel, REG_SECCOUNT0, 	0); stall(1);
	ide_write(channel, REG_LBA0, 		0); stall(1);
	ide_write(channel, REG_LBA1, 		0); stall(1);
	ide_write(channel, REG_LBA2, 		0); stall(1);
	
	ide_write(channel, REG_COMMAND, CMD_IDENTIFY);
	stall(1);
	
	if (ide_read(channel, REG_STATUS) == 0)
		return;
		
	while (1) {
		char sr = ide_read(channel, REG_STATUS);
		if (sr & ATA_SR_DRQ)
			break;
		else if (sr & ATA_SR_ERR)
			panic("***ide error");
	}
	
	for (int i = 0; i < 256; i++) {
		int16_t *buff = (int16_t *)&ispace;
		buff[i] = in16(0x1F0);
	}
	
	for (int i = 0; i < 40; i += 2) {
		char tmp = ispace.ModelNumber[i];
		ispace.ModelNumber[i] = ispace.ModelNumber[i + 1];
		ispace.ModelNumber[i + 1] = tmp;
	}
	
	kprintfln("name: %s", ispace.ModelNumber);
	kprintfln("lba48: %d", ispace.CommandSetSupport.BigLba);

	read_sec(channel, 791);
}

void init_ide(int r0, int r1, int r2, int r3, int r4) {
	
	kprintfln("init ide");
	
	if (r0 == 0 || r0 == 1) r0 = 0x1f0;
	if (r1 == 0 || r1 == 1) r1 = 0x3f6;
	if (r2 == 0 || r2 == 1) r2 = 0x170;
	if (r3 == 0 || r3 == 1) r3 = 0x376;
	
	Channel *a = &channels[0];
	a->base	= r0;
	a->ctrl	= r1;
	a->bmide = r4 + 0;
	
	Channel *b = &channels[1];
	b->base	= r2;
	b->ctrl	= r3;
	b->bmide = r4 + 8;
	
	for (int i = 0; i < 2; i++) {
		detect_device(i, MASTER);
		detect_device(i, SLAVE);
	}
}




















