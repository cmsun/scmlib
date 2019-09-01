关于FATFS逻辑磁盘和物理磁盘的设置

1.物理磁盘的盘符只跟加载顺序有关。比如
retSD = FATFS_LinkDriver(&SD_Driver, SDPath);       //加载SD卡
retUSER = FATFS_LinkDriver(&USER_Driver, USERPath); //加载SPI Flash
则SD卡先被加载，它的物理盘符就是0。SPI Flash后被加载，它的物理盘符就是1。

2._MULTI_PARTITION 宏用来决定一个物理磁盘是否支持多个分区。
如果被定义成0，则FATFS只能识别一个物理磁盘的第一个分区，并且此时候物理盘符
和逻辑盘符是对应的。使用f_mount()时必须使用对应的逻辑盘符，否则会失败。

3.如果MULTI_PARTITION被定义成1，则FATFS就能支持一个物理磁盘的多个分区。
比如SD卡有两个分区而 SPI Flash 有一个分区，并且先加载了SD卡后加载 SPI Flash。
如果这个时候希望给 SPI Flash 分配逻辑盘符0，给SD卡的两个分区分别分配逻辑
盘符1和2，那应该修改VolToPart变量如下：
PARTITION VolToPart[] = {{1, 0}, {0, 0}, {0, 1}};
这样，当使用逻辑盘符"0:"操作的时候，使用0作为index查找到{1, 0}，它表示物理磁
盘1的第0个分区。当使用逻辑盘符"1:"操作的时候，使用1作为index查找到{0, 0},它
表示物理磁盘0的第0个分区。当使用逻辑盘符"2:"操作的时候，使用2作为index查找到
{0, 1},它表示物理磁盘0的第1个分区。这时候应该这样mount：
f_mount(&USERFatFS, "0:", 1)；
f_mount(&SDFatFS, "1:", 1);
f_mount(&SDFatFS, "2:", 1);