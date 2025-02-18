#ifndef __CC_APPLICATION_WIN32_H__
#define __CC_APPLICATION_WIN32_H__

#include "../../include/ccMacros.h"
#include "CCStdC.h"
#include "../CCCommon.h"
#include "../CCApplicationProtocol.h"
#include <string>

NS_CC_BEGIN

class CCRect;

class CC_DLL CCApplication : public CCApplicationProtocol
{
    GEODE_FRIEND_MODIFY
public:
    GEODE_MONOSTATE_CONSTRUCTOR_BEGIN(CCApplication)
    CCApplication();
    virtual ~CCApplication();

    /**
    @brief    Run the message loop.
    */
    RT_REMOVE(  virtual int run();  )
    RT_ADD(     virtual void gameDidSave(); )

    /**
    @brief    Get current applicaiton instance.
    @return Current application instance pointer.
    */
    static CCApplication* sharedApplication();

    static GEODE_DLL CCApplication* get();

    /* override functions */
    virtual void setAnimationInterval(double interval);
    virtual ccLanguageType getCurrentLanguage();
    
    /**
     @brief Get target platform
     */
    virtual TargetPlatform getTargetPlatform();

    RT_ADD(
        virtual void openURL(const char* url);
        virtual int run();
        virtual void setupGLView();
        virtual void platformShutdown();
        void toggleVerticalSync(bool);
        bool getVerticalSyncEnabled() const;
    )

    /**
     *  Sets the Resource root path.
     *  @deprecated Please use CCFileUtils::sharedFileUtils()->setSearchPaths() instead.
     */
    CC_DEPRECATED_ATTRIBUTE void setResourceRootPath(const gd::string& rootResDir);

    /** 
     *  Gets the Resource root path.
     *  @deprecated Please use CCFileUtils::sharedFileUtils()->getSearchPaths() instead. 
     */
    CC_DEPRECATED_ATTRIBUTE const gd::string& getResourceRootPath(void);

    void setStartupScriptFilename(const gd::string& startupScriptFile);
    
    bool getControllerConnected() const;

    const gd::string& getStartupScriptFilename(void)
    {
        return m_startupScriptFilename;
    }

protected:
    HINSTANCE           m_hInstance;
    HACCEL              m_hAccelTable;
    LARGE_INTEGER       m_nAnimationInterval;
    gd::string         m_resourceRootPath;
    gd::string         m_startupScriptFilename;

    static CCApplication * sm_pSharedApplication;
};

NS_CC_END

#endif    // __CC_APPLICATION_WIN32_H__
