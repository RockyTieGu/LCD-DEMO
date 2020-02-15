#ifndef _MASS_H_
#define _MASS_H_
#include "usbd.h"
//#include "mass_storage_class.h"
#include "w55fa93_sic.h"
#include "nvtfat.h"

void mass(NDISK_T *disk, int i32TotalSector);
void NVT_MountMem(void);
#endif
