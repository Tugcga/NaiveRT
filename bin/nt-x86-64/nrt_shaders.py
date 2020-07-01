import win32com.client
from win32com.client import constants as c


app = Application
xsi_factory = XSIFactory


def XSILoadPlugin(in_reg):
    in_reg.Author = "Shekn Itrch"
    in_reg.Name = "NRTShadersPlugin"
    in_reg.Major = 1
    in_reg.Minor = 0
    # Shaders
    in_reg.RegisterShader("NRTLambertian", 1, 0)
    in_reg.RegisterShader("NRTMetal", 1, 0)
    in_reg.RegisterShader("NRTDielectric", 1, 0)

    return True


def XSIUnloadPlugin(in_reg):
    strPluginName = in_reg.Name
    app.LogMessage(str(strPluginName) + str("has been unloaded."), c.siVerbose)
    return True


def add_output_color(shaderDef, name="color"):
    paramOptions = xsi_factory.CreateShaderParamDefOptions()
    paramOptions.SetLongName(name)
    params = shaderDef.OutputParamDefs
    paramDef = params.AddParamDef2("out" + name, c.siShaderDataTypeColor3, paramOptions)
    paramDef.MainPort = False


def add_input_color(paramOptions, params, defaultValue=1, name="color"):
    paramOptions.SetDefaultValue(defaultValue)
    params.AddParamDef(name, c.siShaderDataTypeColor3, paramOptions)


def add_input_float(paramOptions, params, defaultValue=0, name="float", visMin=None, visMax=None):
    paramOptions.SetDefaultValue(defaultValue)
    if visMin is not None and visMax is not None:
        paramOptions.SetSoftLimit(visMin, visMax)
    params.AddParamDef(name, c.siShaderDataTypeScalar, paramOptions)


def no_port_param():
    paramOptions = xsi_factory.CreateShaderParamDefOptions()
    paramOptions.SetAnimatable(True)
    paramOptions.SetTexturable(False)
    paramOptions.SetReadOnly(False)
    paramOptions.SetInspectable(True)
    return paramOptions


# ---------------------------------------------------------
# ---------------------Lambertian--------------------------

def NRTShadersPlugin_NRTLambertian_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "NaiveRT")
    in_ctxt.SetAttribute("DisplayName", "nrtLambertian")
    return True


def NRTShadersPlugin_NRTLambertian_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyVolume)
    shaderDef.AddShaderFamily(c.siShaderFamilySurfaceMat)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_color(no_port_param(), params, 0.8, "albedo")

    # Output Parameter: out
    add_output_color(shaderDef, "BSDF")

    # next init ppg
    ppgLayout = shaderDef.PPGLayout
    ppgLayout.AddGroup("Parameters")
    ppgLayout.AddItem("albedo", "Albedo")
    ppgLayout.EndGroup()

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("NaiveRT")
    rendererDef.SymbolName = "Lambertian"
    return True


# ---------------------------------------------------------
# ------------------------Metal-----------------------------

def NRTShadersPlugin_NRTMetal_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "NaiveRT")
    in_ctxt.SetAttribute("DisplayName", "nrtMetal")
    return True


def NRTShadersPlugin_NRTMetal_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyVolume)
    shaderDef.AddShaderFamily(c.siShaderFamilySurfaceMat)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_color(no_port_param(), params, 0.8, "albedo")
    add_input_float(no_port_param(), params, 0.2, "fuzz", 0.0, 1.0)

    # Output Parameter: out
    add_output_color(shaderDef, "BSDF")

    # next init ppg
    ppgLayout = shaderDef.PPGLayout
    ppgLayout.AddGroup("Parameters")
    ppgLayout.AddItem("albedo", "Albedo")
    ppgLayout.AddItem("fuzz", "Fuzz")
    ppgLayout.EndGroup()

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("NaiveRT")
    rendererDef.SymbolName = "Metal"
    return True


# ---------------------------------------------------------
# ---------------------Dielectric--------------------------

def NRTShadersPlugin_NRTDielectric_1_0_DefineInfo(in_ctxt):
    in_ctxt.SetAttribute("Category", "NaiveRT")
    in_ctxt.SetAttribute("DisplayName", "nrtDielectric")
    return True


def NRTShadersPlugin_NRTDielectric_1_0_Define(in_ctxt):
    shaderDef = in_ctxt.GetAttribute("Definition")
    shaderDef.AddShaderFamily(c.siShaderFamilyVolume)
    shaderDef.AddShaderFamily(c.siShaderFamilySurfaceMat)

    # Input Parameter: input
    params = shaderDef.InputParamDefs

    # parameters
    add_input_float(no_port_param(), params, 1.4, "ref_idx", 0.0, 4.0)

    # Output Parameter: out
    add_output_color(shaderDef, "BSDF")

    # next init ppg
    ppgLayout = shaderDef.PPGLayout
    ppgLayout.AddGroup("Parameters")
    ppgLayout.AddItem("ref_idx", "Refraction Index")
    ppgLayout.EndGroup()

    # Renderer definition
    rendererDef = shaderDef.AddRendererDef("NaiveRT")
    rendererDef.SymbolName = "Dielectric"
    return True
