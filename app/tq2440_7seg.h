#define _7SEG_MODE_HEX_VALUE		1
#define _7SEG_MODE_PATTERN		0
#define SEG_MAJOR			235
#define LCD_IOCTL_MAGIC			SEG_MAJOR
#define LCD_IO(nr)			_IO(LCD_IOCTL_MAGIC,nr)
#define LCD_IOR(nr,size)		_IOR(LCD_IOCTL_MAGIC,nr,size)
#define LCD_IOW(nr,size)		_IOW(LCD_IOCTL_MAGIC,nr,size)
#define LCD_IOWR(nr,size)		_IOWR(LCD_IOCTL_MAGIC,nr,size)
#define _7SEG_IOCTL_ON			LCD_IO(  0x70)	
/* Ãö³¬ 7 Segment LED */
#define _7SEG_IOCTL_OFF			LCD_IO(  0x71)
/* ³]©w 7 Segment LED*/
#define _7SEG_IOCTL_SET			LCD_IOW( 0x72,	_7seg_info_t)

#define _7SEG_D5_INDEX			8	/* Segment œsž¹D5 (1)	*/
#define _7SEG_D6_INDEX			4	/* Segment œsž¹D6 (2)	*/
#define _7SEG_D7_INDEX			2	/* Segment œsž¹D7 (3)	*/
#define _7SEG_D8_INDEX			1
#define _7SEG_ALL			(_7SEG_D5_INDEX|_7SEG_D6_INDEX|_7SEG_D7_INDEX|_7SEG_D8_INDEX)

#define OK 1
#define UM 0
typedef struct _7Seg_Info {			
	unsigned char	Mode ;			
	unsigned char	Which ;				 
						/* or nibble 				*/		
						/* šÒŠp : D5, D8Åã¥Ü58 			*/
						/* Mode  = _7SEG_MODE_HEX_VALUE 	*/
						/* Which = _7SEG_D5_INDEX | _7SEG_D8_INDEX */
						/* Value = 0x58 			*/
						
	unsigned long	Value;			/* ŠUºØModeªº¿é€J­È */
} _7seg_info_t ;
