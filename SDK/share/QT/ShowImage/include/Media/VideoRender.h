/**
* @file   VideoRender.h
* @brief  显示库，将YUV显示出图像.
* @author 
* @par    Copyright (c):
*         All Rights Reserved
* @date   2015-08-17
* @note   接口是线程不安全的。
* @version 1.00.1
*/
#ifndef __VIDEORENDER_H_
#define __VIDEORENDER_H_

#ifdef  __cplusplus
extern "C"{
#endif  // end #ifdef  __cplusplus

#ifdef WIN64
#undef WIN32
#endif

/** 编译选项 */
#ifdef WIN32  // win64位下，会报和def重复定义的警告
#  ifdef _USRDLL // 动态库导出
#    ifdef VIDEORENDER_EXPORTS
#		define VRENDER_API __declspec(dllexport)
#	 else
#		define VRENDER_API __declspec(dllimport)
#	 endif
#  else
#    define VRENDER_API
#endif 
#else
#	define VRENDER_API
#endif //end #ifdef WIN32


#if (defined (WIN32) || defined(WIN64))
#   define CALLMETHOD __stdcall
#	define CALLBACK	__stdcall
#else
#	define CALLMETHOD
#	define CALLBACK
#endif // end #if (defined (WIN32) || defined(WIN64))

#define IN
#define OUT

typedef void* VR_HANDLE;     // 播放API使用的句柄 
typedef void* VR_HWND;       // 窗口句柄 
typedef void* VR_HDC;        // 绘图句柄 
typedef void* VR_HFONT;      // 字体句柄 

/**
* @enum tagRenderMode
* @brief 视频显示模式枚举定义
* @attention 无
*/
typedef enum tagVideoRenderMode
{
#if (defined (WIN32) || defined(WIN64))
	VR_MODE_D3D = 0,             // 使用D3D的方式进行显示 
	VR_MODE_GDI,                 // 使用GDI显示模式 
	VR_MODE_OPENGL,
#else
	VR_MODE_OPENGLX,			 // 使用Opengl的方式进行显示 
	VR_MODE_X11,				 // 使用X11的方式进行显示 
	VR_MODE_OPENGL,				 // 同 VR_MODE_OPENGLX
#endif // end #if (defined (WIN32) || defined(WIN64))
}VR_MODE_E;

/**
* @enum tagVideoRenderErr
* @brief 接口返回值
* @attention 无
*/
typedef enum tagVideoRenderErr
{
	VR_SUCCESS,
	VR_ILLEGAL_PARAM,			 // 非法参数
	VR_ERR_ORDER,                // 调用接口顺序错误
	VR_NO_MEMORY,				 // 内存不足
	VR_NOT_SUPPORT,              // 不支持，或是缺少系统库，或是不支持输入的参数值
	VR_D3D_PRESENT_FAILED,       // D3D显示错误 
	VR_GDI_CREATE_OBJ_FAILED,    // GGI创建对象失败 
	VR_DEFAULT_FONT_NOT_EXIST,   // 默认字体不存在 
	VR_GDI_PRESENT_FAILED,       // GDI显示错误 
}VR_ERR_E;

/**
* @enum tagVideoRenderPixelType
* @brief 帧像素类型
* @attention 无
*/
typedef enum tagVideoRenderPixelType
{
	VR_PIXEL_FMT_NONE = -1,
	VR_PIXEL_FMT_YUV420P,				    ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
	VR_PIXEL_FMT_RGB24,                     ///< packet RGB 8:8:8, 24bpp, RGBRGB...
	VR_PIXEL_FMT_MONO8,						///< mono8
	VR_PIXEL_FMT_XRGB32,				    ///< X8R8G8B8, 从低字节到高字节分别是: B,G,R,X, X仅用于占位, 不用于显示
	
}VR_PIXEL_TYPE_E;

/**
* @enum VR_OPENGL_PROFILE_E
* @brief OpenGL上下文模式
*/
typedef enum tagOpenGLProfile
{
	VR_OPENGL_PROFILE_AUTO,					/// 自动模式, 优先使用Core profile, 若驱动版本太低则使用Compatibility profile
	VR_OPENGL_PROFILE_COMPAT,				/// Compatibility profile, 只使用固定管线
	VR_OPENGL_PROFILE_CORE,					/// Core profile, 只使用可编程管线
}VR_OPENGL_PROFILE_E;

/**
* @enum VR_FONT_MODE_E
* @brief 字体模式
*/
typedef enum tagFontMode
{
	VR_FONT_MODE_FREETYPE,				/// freetype, 适用于OpenGL渲染模式
	VR_FONT_MODE_GDI,					/// GDI, 适用于GDI渲染模式, 仅支持windows系统
}VR_FONT_MODE_E;

/**
* @enum VR_LINE_MODE_E
* @brief 画线模式
*/
typedef enum tagLineMode
{
	VR_LINE_MODE_LINES,					/// 每2个点绘制一条直线, 若点数为奇数, 忽略最后一个点
	VR_LINE_MODE_LINE_STRIP,			/// 依次连接各个点, 当点数为N时, 直线数为N-1
	VR_LINE_MODE_LINE_LOOP,				/// 依次连接各个点, 并连接首尾点, 当点数为N时, 直线数为N
}VR_LINE_MODE_E;

/**
* @struct tagVideoRenderOpenParam
* @brief 打开视频显示所需要的参数。供VR_Open接口使用。
* @attention 无
*/
typedef struct tagVideoRenderOpenParam
{
	VR_HWND hWnd;				// 窗口句柄
	VR_MODE_E eVideoRenderMode; // 显示模式
	int nWidth;					// 视频宽
	int nHeight;				// 视频高
}VR_OPEN_PARAM_S;

/**
* @struct tagVideoRenderFrame
* @brief 显示视频所需要的参数。供VR_RenderFrame接口使用。
* @attention 无
*/
typedef struct tagVideoRenderFrame
{
#define NUM_DATA_POINTS 4
	unsigned char* data[NUM_DATA_POINTS];      // 根据format决定，YUV420P时每个分量分别表示指向Y，U，V缓冲，RGB时，只使用data[0]，指向RGB缓冲
	int stride[NUM_DATA_POINTS];			   // 每个分量的跨距
	int nWidth;								   // 视频宽,为了防止显示过程中修改分辨率
	int nHeight;							   // 视频高,为了防止显示过程中修改分辨率
	VR_PIXEL_TYPE_E format;
}VR_FRAME_S;


/**
* @struct tagVideoRenderRenderParam
* @brief 显示视频所需要的参数。供VR_RenderEx接口使用。
* @attention 不建议使用,兼容老版本而保留
*/
typedef struct tagVideoRenderRenderParam
{
	unsigned char* data[3];    // 每个分量分别表示指向Y，U，V缓冲
	int stride[3];			   // 每个分量的跨距
	int nWidth[3];			   // 每个分量的宽
	int nHeight[3];			   // 每个分量的高
}VR_RENDER_PARAM_S;

#if (defined(WIN32) || defined (WIN64))
typedef unsigned int VR_COLOR;
#else
typedef unsigned int VR_COLOR;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;

#define GetRValue(rgb) ((BYTE)(rgb))
#define GetGValue(rgb) ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb) ((BYTE)((rgb)>>16))
#define RGB(r,g,b) ((VR_COLOR)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#endif

/**
* @struct tagFontInfo
* @brief 字体参数。供VR_CreateFont接口使用。
* @attention 无
*/
typedef struct tagFontInfo
{
	unsigned int nFontSize;    ///< 字体大小
}VR_FONT_INFO;

typedef struct tagFontInfoEx
{
	unsigned int fontSize;		///< 字体大小
	int			 fontMode;		///< 字体创建方式, 0-freetype, 1-GDI
	const char*  fontPath;		///< 字体文件路径或者名称, 若为空则使用默认字体, 默认字体可能只支持拉丁字母不支持CJK
								///< 当fontMode为0时, 表示字体路径; 当fontMode为1时, 表示字体名称
	int			 reserved[12];
} VR_FONT_INFO_EX;

/**
* @struct tagVRPoint
* @brief 点坐标
* @attention 无
*/
typedef struct tagVRPoint {
   long x;
   long y;
} VR_POINT;

typedef struct tagVRPointF {
	float x;
	float y;
} VR_POINT_F;

/**
* @struct tagVRRect
* @brief 矩形
* @attention 无
*/
typedef struct tagVRRect
{
	int    left;
	int    top;
	int    right;
	int    bottom;
}VR_RECT;

/**
* @struct VR_LINE_STYLE
* @brief 直线绘制样式
*/
typedef struct tagVRLineStyle
{
	int				width;			///< 线宽, 单位: 像素
	VR_COLOR		color;			///< 颜色, RGB
	VR_LINE_MODE_E	mode;			///< 绘制模式
	int				reserved[13];
}VR_LINE_STYLE;

/**
* @struct VR_OPENGL_OPTION
* @brief OpenGL选项
*/
typedef struct tagVROpenGLOption
{
	VR_OPENGL_PROFILE_E profile;		///< 模式, 默认是自动模式(VR_OPENGL_PROFILE_AUTO)
	int					reserved[15];
}VR_OPENGL_OPTION;

/**
*  @brief 设置OpenGL选项，对之后所有的VR_Open函数生效
*  @param[in]  option：OpenGL选项
*/
VRENDER_API void CALLMETHOD VR_SetOpenGLOption(IN const VR_OPENGL_OPTION* option);

/**
*  @brief  生成一个播放对象
*  @param[in]  pParam：VR_OPEN_PARAM_S结构体。生成播放对象所需要的参数。
*  @param[out] pHandle：VR_HANDLE句柄。输出生成的播放的对象。
*  @Return:    VR_ERR_E:播放接口返回值
*  - VR_SUCCESS 表示执行成功
*  - 其他值见 VR_ERR_E枚举
*
*  @note   执行成功后，需要调用VR_Close来释放资源。
*  @see    VR_ERR_E，VR_Close
*/
VRENDER_API VR_ERR_E CALLMETHOD VR_Open(IN VR_OPEN_PARAM_S* pParam, OUT VR_HANDLE* pHandle);

/**
*  @brief  将输入的一帧YUV显示出图像
*  @param[in] handle：VR_HANDLE句柄，由VR_Open创建。
*  @param[in] param：VR_FRAME_S结构体。
*  @param[in] pEnlargeRect：VR_RECT结构体。放大矩形区域。原图大小显示，不需要放大时，填NULL；
*  @Return:    VR_ERR_E:播放接口返回值
*  - VR_SUCCESS 表示执行成功
*  - 其他值见 VR_ERR_E枚举
*
*  @note   输入一帧即显示，播放速度由外部输入速度控制。
*  @see    VR_ERR_E
*/
VRENDER_API VR_ERR_E CALLMETHOD VR_RenderFrame(IN VR_HANDLE handle, IN VR_FRAME_S* param, IN VR_RECT* pEnlargeRect);


/**
*  @brief  关闭播放对象
*  @param[in] handle：VR_HANDLE句柄，由VR_Open创建。
*  @Return:    VR_ERR_E:播放接口返回值
*  - VR_SUCCESS 表示执行成功
*  - 其他值见 VR_ERR_E枚举
*
*  @note   和VR_Open配对使用。
*  @see    VR_ERR_E，VR_Open
*/
VRENDER_API VR_ERR_E CALLMETHOD VR_Close(IN VR_HANDLE handle);


typedef void (CALLBACK *fnDrawCallback)(VR_HWND hWindow, VR_HDC hDrawHandle, void* pUserData);
/**
*  @brief  设置GDI回调, 可以使用GDI在画面中绘制叠加显示
*  @param[in] handle：VR_HANDLE句柄，由VR_Open创建。
*  @param[in] callback：回调函数
*  @param[in] pUserData：用户回调参数
*  @Return:    VR_ERR_E:播放接口返回值
*  - VR_SUCCESS 表示执行成功
*  - 其他值见 VR_ERR_E枚举
*/
VRENDER_API VR_ERR_E CALLMETHOD VR_SetDrawCallback(IN VR_HANDLE handle, IN fnDrawCallback callback, IN void* pUserData);


/**
*  @brief 创建字体句柄
*  @param[in] pFontInfo：字体信息，包括颜色和字体大小。
*  @param[out] hFont：获得字体句柄
*  @Return:    VR_ERR_E:播放接口返回值
*  - VR_SUCCESS 表示执行成功
*  - 其他值见 VR_ERR_E枚举
*
*  @note 画图辅助函数。
*/
VRENDER_API VR_ERR_E CALLMETHOD VR_CreateFont(IN VR_FONT_INFO* pFontInfo, OUT VR_HFONT* hFont); 
VRENDER_API VR_ERR_E CALLMETHOD VR_CreateFontEx(IN VR_FONT_INFO_EX* pFontInfo, OUT VR_HFONT* hFont);

/**
*  @brief 销毁字体句柄
*  @param[in] hFont：字体句柄，由VR_CreateFont创建
*  @Return:    VR_ERR_E:播放接口返回值
*  - VR_SUCCESS 表示执行成功
*  - 其他值见 VR_ERR_E枚举
*
*  @note 画图辅助函数。
*/
VRENDER_API VR_ERR_E CALLMETHOD VR_DestroyFont(IN VR_HFONT hFont);

/**
*  @brief 绘字。支持\n符回车显示
*  @param[in] hFont：字体。
*  @param[in] hWindow：窗口句柄，由fnDrawCallback绘图回调输出
*  @param[in] hDrawHandle:绘图句柄，由fnDrawCallback绘图回调输出
*  @param[in] lpText:文字缓存。当使用GDI渲染时使用ANSI编码, 其他模式使用utf8编码
*  @param[in] nTextLen:文字长度, -1表示自动计算长度
*  @param[in] color:字体颜色
*  @param[in] pointStart:开始绘制字符串的位置
*  @Return:    VR_ERR_E:播放接口返回值
*  - VR_SUCCESS 表示执行成功
*  - 其他值见 VR_ERR_E枚举
*
*  @note 画图辅助函数。
*/
VRENDER_API VR_ERR_E CALLMETHOD VR_DrawText(IN VR_HFONT hFont, 
										IN VR_HWND hWindow, 
										IN VR_HDC hDrawHandle, 
										IN const char* lpText, 
										IN int nTextLen, 
										IN VR_COLOR color,
										IN VR_POINT* pointStart);


/**
*  @brief  将输入的一帧YUV显示出图像
*  @param[in] handle：VR_HANDLE句柄，由VR_Open创建。
*  @param[in] param：VR_RENDER_PARAM_S结构体。显示所需要的参数。
*  @param[in] pEnlargeRect：VR_RECT结构体。放大矩形区域。原图大小显示，不需要放大时，填NULL；
*  @attention 此接口不建议使用，建议使用VR_RenderFrame替代。为兼容老版本的使用而保留。
*  @Return:    VR_ERR_E:播放接口返回值
*  - VR_SUCCESS 表示执行成功
*  - 其他值见 VR_ERR_E枚举
*
*  @note   输入一帧即显示，播放速度由外部输入速度控制。
*  @see    VR_ERR_E
*/
VRENDER_API VR_ERR_E CALLMETHOD VR_RenderEx(IN VR_HANDLE handle, IN VR_RENDER_PARAM_S* param, IN VR_RECT* pEnlargeRect);

/**
*  @brief  将输入的一帧YUV显示出图像
*  @param[in] handle：VR_HANDLE句柄，由VR_Open创建。
*  @param[in] param：VR_RENDER_PARAM_S结构体。显示所需要的参数。
*  @Return:    VR_ERR_E:播放接口返回值
*  - VR_SUCCESS 表示执行成功
*  - 其他值见 VR_ERR_E枚举
*
*  @attention 此接口不建议使用，建议使用VR_RenderFrame替代。为兼容老版本的使用而保留。
*  @note   输入一帧即显示，播放速度由外部输入速度控制。
*  @see    VR_ERR_E
*/
VRENDER_API VR_ERR_E CALLMETHOD VR_Render(IN VR_HANDLE handle, IN VR_RENDER_PARAM_S* param);

/**
*  @brief 连续绘制多条直线, 可用于绘制直线、折线、多边形、矩形、圆等图形
*  @param[in] hWindow：窗口句柄，由fnDrawCallback绘图回调输出
*  @param[in] hDrawHandle：绘图句柄，由fnDrawCallback绘图回调输出
*  @param[in] points：点坐标数组
*  @param[in] count：点数量
*  @param[in] style：直线绘制样式
*  @Return:
*  - VR_SUCCESS 表示执行成功
*  - 其他值见 VR_ERR_E枚举
**/
VRENDER_API VR_ERR_E CALLMETHOD VR_DrawLines(IN VR_HWND hWindow, IN VR_HDC hDrawHandle, IN const VR_POINT_F* points, IN int count, IN VR_LINE_STYLE* style);


#ifdef  __cplusplus
}
#endif // end #ifdef  __cplusplus

#endif // end #ifndef __VIDEORENDER_H_
