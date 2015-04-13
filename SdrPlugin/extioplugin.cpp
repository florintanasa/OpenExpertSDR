#include "extioplugin.h"

ExtIOPlugin* ExtIOPlugin::plugin_instance = NULL;

ExtIOPlugin::ExtIOPlugin()
{
    ExtIOMode = false;
    EXtIOOpen = false;
    sdrmode = -1;
    IFLimitLow = 0;
    IFLimitHigh = 0;
    memset(&routs, 0, sizeof(ExtIORouts));
}

void ExtIOPlugin::ExtIOPluginInit(QLibrary *plugin)
{
    ExtIOMode = true;
    plugin_instance = this;

    routs.ShowGUI = (ExtIO_ShowGUI)plugin->resolve("ShowGUI");
    routs.InitHW = (ExtIO_InitHW)plugin->resolve("InitHW");
    routs.OpenHW = (ExtIO_OpenHW)plugin->resolve("OpenHW");
    routs.CloseHW = (ExtIO_CloseHW)plugin->resolve("CloseHW");
    routs.StartHW = (ExtIO_StartHW)plugin->resolve("StartHW");
    routs.StopHW = (ExtIO_StopHW)plugin->resolve("StopHW");
    routs.SetHWLO = (ExtIO_SetHWLO)plugin->resolve("SetHWLO");
    routs.GetStatus = (ExtIO_GetStatus)plugin->resolve("GetStatus");
    routs.SetCallback = (ExtIO_SetCallback)plugin->resolve("SetCallback");
    // ext routs
    routs.GetHWLO = (ExtIO_GetHWLO)plugin->resolve("GetHWLO");
    routs.TuneChanged = (ExtIO_TuneChanged)plugin->resolve("TuneChanged");
    routs.IFLimitsChanged = (ExtIO_IFLimitsChanged)plugin->resolve("IFLimitsChanged");
    routs.GetTune = (ExtIO_GetTune)plugin->resolve("GetTune");
    routs.GetMode = (ExtIO_GetMode)plugin->resolve("GetMode");
    routs.ModeChanged = (ExtIO_ModeChanged)plugin->resolve("ModeChanged");
    routs.GetHWSR = (ExtIO_GetHWSR)plugin->resolve("GetHWSR");
    routs.HideGUI = (ExtIO_HideGUI)plugin->resolve("HideGUI");
    routs.RawDataReady = (ExtIO_RawDataReady)plugin->resolve("RawDataReady");
    routs.GetFilters = (ExtIO_GetFilters)plugin->resolve("GetFilters");
    // hdsdr routs
    routs.SetModeRxTx = (ExtIOext_SetModeRxTx)plugin->resolve("SetModeRxTx");
    routs.ActivateTx = (ExtIOext_ActivateTx)plugin->resolve("ActivateTx");
    routs.VersionInfo = (ExtIOext_VersionInfo)plugin->resolve("VersionInfo");
    routs.SetPreamp = (ExtIOext_SetPreamp)plugin->resolve("SetPreamp");
    #  if defined (__GNUC__) && defined(__unix__)
    typedef void (*ExtIOext_VersionInfo)(char *name, int ver_major, int ver_minor);
    #  elif defined WIN32
    typedef __stdcall void (*ExtIOext_VersionInfo)(char *name, int ver_major, int ver_minor);
    #  endif


    if(init_hw())
    {
        version_info("HDSDR", 2, 70);
    }
    set_callback();
    activate_tx(12345678, 87654321); // random ints in first pass
    activate_tx(-1, -1); // -1 in second pass
}

void ExtIOPlugin::ExtIOPluginDeinit()
{
    if(plugin_instance == this)
        plugin_instance = NULL;
    ExtIOMode = false;
}

void ExtIOPlugin::ShowGUI()
{
    if(routs.ShowGUI)
        routs.ShowGUI();
}

int ExtIOPlugin::StartHW(long freq)
{
    int res = 0;
    if(routs.StartHW)
    {
        res = routs.StartHW(freq);
        EXtIOOpen = true;
    }

    return res;
}

void ExtIOPlugin::StopHW()
{
    if(routs.StopHW)
        routs.StopHW();
    EXtIOOpen = false;
}

int ExtIOPlugin::SetHWLO(long LOfreq)
{
    int res = 0;
    if(routs.SetHWLO)
        res = routs.SetHWLO(LOfreq);
    return res;
}

long ExtIOPlugin::GetHWLO()
{
    if(routs.GetHWLO)
        return routs.GetHWLO();

    return 0;
}

void ExtIOPlugin::IFLimitsChanged(long low, long high)
{
    if(routs.IFLimitsChanged)
        routs.IFLimitsChanged(low, high);
}

long ExtIOPlugin::GetTune()
{
    if(routs.GetTune)
        return routs.GetTune();

    return 0;
}

SDRMODE ExtIOPlugin::GetMode()
{
    char res = 0;
    SDRMODE mode = LSB;
    if(routs.GetMode)
        res = routs.GetMode();

    switch (res) {
    case 'A':
        mode = AM;
        break;

    case 'F':
        mode = FMN;
        break;

    case 'L':
        mode = LSB;
        break;

    case 'U':
        mode = USB;
        break;

    case 'C':
        mode = CWL;
        break;

    case 'D':
        mode = DRM;
        break;
    }

    return mode;
}

void ExtIOPlugin::SetModeRxTx(HwModeRxTx mode)
{
    if(routs.SetModeRxTx)
        routs.SetModeRxTx((int)mode);
}

bool ExtIOPlugin::IsExtIOMode()
{
    return ExtIOMode;
}

bool ExtIOPlugin::IsExtIOOpen()
{
    return EXtIOOpen;
}

QString ExtIOPlugin::getInfoStr()
{
    return InfoStr;
}

bool ExtIOPlugin::IsExtIO(QLibrary *plugin)
{
    if(plugin == NULL)
        return false;

    QFileInfo fi(plugin->fileName());
    QString filename = fi.fileName();
    if(filename.indexOf("ExtIO_", 0, Qt::CaseInsensitive) == -1)
        return false;

    if (plugin->resolve("InitHW") == NULL)
        return false;

    if (plugin->resolve("OpenHW") == NULL)
        return false;

    if (plugin->resolve("StartHW") == NULL)
        return false;

    if (plugin->resolve("StopHW") == NULL)
        return false;

    if (plugin->resolve("CloseHW") == NULL)
        return false;

    if (plugin->resolve("SetHWLO") == NULL)
        return false;

    if (plugin->resolve("GetStatus") == NULL)
        return false;

    if (plugin->resolve("SetCallback") == NULL)
        return false;

    return true;
}

bool ExtIOPlugin::init_hw()
{
    bool res = false;
    if(routs.InitHW)
    {
        #  if defined (__GNUC__) && defined(__unix__)
        QByteArray name(PATH_MAX, 0);
        QByteArray model(PATH_MAX, 0);
        #  elif defined WIN32
        QByteArray name(MAX_PATH, 0);
        QByteArray model(MAX_PATH, 0);
        #  endif
//        QByteArray name(MAX_PATH, 0);
//        QByteArray model(MAX_PATH, 0);
        int type = 0;
        QMessageBox msgBox;
        res = routs.InitHW(name.data(), model.data(), type);
        if(res)
        {
            InfoStr = QString::fromLatin1(name.data()) + "(" +  QString::fromLatin1(model.data()) + ")";
            if(type != 4)
            {
                msgBox.setText("ExtIO Plugin for " + InfoStr + "is not supported.\nSupport receive IQ data via soundcard only.");
                msgBox.exec();
            }
        }
        else
        {
            msgBox.setText("Init hardware failed.");
            msgBox.exec();
        }
    }
    return res;
}

bool ExtIOPlugin::OpenHW()
{
    bool res = false;
    if(routs.OpenHW)
        res = routs.OpenHW();

    if(!res)
    {
        QMessageBox msgBox;
        msgBox.setText("Cant open " + InfoStr + " hardware.");
        msgBox.exec();
    }

    return res;
}

void ExtIOPlugin::CloseHW()
{
    if(routs.CloseHW)
        routs.CloseHW();
}

void ExtIOPlugin::set_callback()
{
    if(routs.SetCallback)
        routs.SetCallback(extIOCallback);
}

void ExtIOPlugin::UbdateIfLimits(long dds_freq, int sample_rate)
{
    if((dds_freq < 0) || (sample_rate <= 0))
        return;

    long half_filter = sample_rate / 2;
    long new_if_l = dds_freq - half_filter;
    long new_if_h = dds_freq + half_filter;

    if((new_if_l != IFLimitLow) || (new_if_h != IFLimitHigh))
    {
        IFLimitLow = new_if_l;
        IFLimitHigh = new_if_h;
        IFLimitsChanged(IFLimitLow, IFLimitHigh);
    }
}

int ExtIOPlugin::activate_tx(int magicA, int magicB)
{
    if(routs.ActivateTx)
        return routs.ActivateTx(magicA, magicB);
    return -1;
}

void ExtIOPlugin::version_info(const char *name, int ver_major, int ver_minor)
{
    if(routs.VersionInfo)
        routs.VersionInfo(name, ver_major, ver_minor);
}
void ExtIOPlugin::SetPreamp(int db)
{
    if(routs.SetPreamp)
    {
        routs.SetPreamp(db);
    }
}

void ExtIOPlugin::extIOCallback(int cnt, int status, float IQoffs, short IQdata[])
{
    Q_UNUSED(cnt);
    Q_UNUSED(IQoffs);
    Q_UNUSED(IQdata);
    if((plugin_instance != NULL) && (cnt < 0))
        QMetaObject::invokeMethod(plugin_instance, "OnExtIOCallback",  Qt::QueuedConnection,  Q_ARG(int, status));
}

void ExtIOPlugin::OnModeChanged(int mode)
{
    SDRMODE sdr_mode = (SDRMODE)mode;
    if(sdrmode == mode)
        return;

    if(routs.ModeChanged)
    {
        char cm;

        switch (sdr_mode) {
        case DSB:
        case SAM:
        case AM:
            cm = 'A';
            break;

        case FMN:
            cm = 'F';
            break;

        case DIGL:
        case LSB:
            cm = 'L';
            break;

        case DIGU:
        case USB:
            cm = 'U';
            break;

        case CWL:
        case CWU:
            cm = 'C';
            break;

        case DRM:
            cm = 'D';
            break;

        default:
            cm = 0;
        }

        if(cm != 0)
            routs.ModeChanged(cm);
    }
}

void ExtIOPlugin::OnTuneChanged(int freq)
{
    if(IsExtIOOpen())
        if(routs.TuneChanged)
            routs.TuneChanged(freq);
}

void ExtIOPlugin::OnExtIOCallback(int status)
{
    int mode;
    long freq;

    switch (status) {
    case ecsChangedLO:
        freq = GetHWLO();
        emit DDSChanged(freq);
        break;

    case ecsChangedMODE:
        mode = GetMode();
        sdrmode = mode;
        emit ChangeMode(mode);
        break;

    case ecsStart:
        emit Start(true);
        break;

    case ecsStop:
        emit Start(false);
        break;

    case ecsChangedTUNE:
        freq = GetTune();
        emit TuneChanged(freq);
        break;

    case ecsRXRequest:
        emit Ptt(false);
        break;

    case ecsTXRequest:
        emit Ptt(true);
        break;

    default:
        break;
    }
}
