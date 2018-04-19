#pragma once

/*
 *  宏定义
 */

#ifdef _MSC_VER
    #define __THIS_FUNCTION__   __FUNCTION__
#endif

#ifdef __linux
    #define __THIS_FUNCTION__   __PRETTY_FUNCTION__
#endif

#define X_FAILED_JUMP(Condition) \
    do  \
    {   \
        if (!(Condition))   \
            goto Exit0;     \
    } while (false)

#define X_ASSERT_JUMP(Condition) \
    do  \
    {   \
        if (!(Condition))   \
		{					\
			assert(false);	\
            goto Exit0;     \
		}					\
    } while (false)

#define X_FAILED_RET_CODE(Condition, nCode) \
    do  \
    {   \
        if (!(Condition))   \
        {   \
            nResult = nCode;    \
            goto Exit0;         \
        }   \
    } while (false)

#define X_SUCCESS_JUMP(Condition) \
    do  \
    {   \
        if (Condition)      \
            goto Exit1;     \
    } while (false)
	
#define XCOM_FAILED_JUMP(Condition) \
    do  \
    {   \
        if (FAILED(Condition))   \
            goto Exit0;     \
    } while (false)
	
#define XLOG_FAILED_JUMP(Condition) \
    do  \
    {   \
        if (!(Condition))   \
        {                   \
            LogFormat(            \
                eLogDebug,  \
                    "XLOG_FAILED_JUMP(%s) at %s:%d in %s", #Condition, __FILE__, __LINE__, __THIS_FUNCTION__   \
            );              \
            goto Exit0;     \
        }                   \
    } while (false)

#define X_DELETE_ARRAY(pArray)     \
    do  \
    {   \
        if (pArray)                 \
        {                           \
            delete[] (pArray);      \
            (pArray) = NULL;        \
        }                           \
    } while (false)


#define X_DELETE(p)    \
    do  \
    {   \
        if (p)              \
        {                   \
            delete (p);     \
            (p) = NULL;     \
        }                   \
    } while (false)

#define X_FREE(pvBuffer) \
    do  \
    {   \
        if (pvBuffer)               \
        {                           \
            free(pvBuffer);         \
            (pvBuffer) = NULL;      \
        }                           \
    } while (false)

#define X_COM_RELEASE(pInterface) \
    do  \
    {   \
        if (pInterface)                 \
        {                               \
            (pInterface)->Release();    \
            (pInterface) = NULL;        \
        }                               \
    } while (false)

#define X_FILE_CLOSE(p)    \
    do  \
    {   \
        if  (p) \
        {       \
            fclose(p);  \
            (p) = NULL; \
        }   \
    } while (false)

#define X_FD_CLOSE(p)    \
    do  \
    {   \
        if  ((p) >= 0) \
        {       \
            close(p);  \
            (p) = -1; \
        }   \
    } while (false)

/*
 * 用来取代MAKE_LONG, MAKE_WORD
 */
#define MAKE_DWORD(a, b)      ((DWORD)(((WORD)(((DWORD)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD)(b)) & 0xffff))) << 16))
#define MAKE_WORD(a, b)      ((WORD)(((BYTE)(((DWORD)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD)(b)) & 0xff))) << 8))

#ifdef __linux
    #define LOWORD(l)           ((WORD)(((DWORD)(l)) & 0xffff))
    #define HIWORD(l)           ((WORD)((((DWORD)(l)) >> 16) & 0xffff))
    #define LOBYTE(w)           ((BYTE)(((DWORD)(w)) & 0xff))
    #define HIBYTE(w)           ((BYTE)((((DWORD)(w)) >> 8) & 0xff))
#endif
