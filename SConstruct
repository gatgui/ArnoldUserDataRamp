import sys
import glob
import excons
import excons.config
from excons.tools import arnold


env = excons.MakeBaseEnv()

arniver = arnold.Version(asString=False)
if arniver[0] < 4 or (arniver[0] == 4 and (arniver[1] < 2 or (arniver[1] == 2 and arniver[2] < 12))):
  print("Arnold 4.2.12.0 or above required")
  sys.exit(1)

def toMayaName(name):
  spl = name.split("_")
  return spl[0] + "".join(map(lambda x: x[0].upper() + x[1:], spl[1:]))

prefix = excons.GetArgument("prefix", "gf_")
name = "%suser_data_ramp" % prefix
rampf_maya_name = toMayaName(prefix + "user_data_ramp_f")
rampv_maya_name = toMayaName(prefix + "user_data_ramp_v")
rampc3_maya_name = toMayaName(prefix + "user_data_ramp_c3")
opts = {"PREFIX": prefix,
        "USERDATARAMPF_MAYA_NODENAME": rampf_maya_name,
        "USERDATARAMPV_MAYA_NODENAME": rampv_maya_name,
        "USERDATARAMPC3_MAYA_NODENAME": rampc3_maya_name}

GenerateMtd = excons.config.AddGenerator(env, "mtd", opts)
GenerateMayaAE = excons.config.AddGenerator(env, "mayaAE", opts)
mtd = GenerateMtd("src/%s.mtd" % name, "src/user_data_ramp.mtd.in")
ae  = GenerateMayaAE("maya/%sTemplate.py" % rampf_maya_name, "maya/UserDataRampFTemplate.py.in")
ae += GenerateMayaAE("maya/%sTemplate.py" % rampv_maya_name, "maya/UserDataRampVTemplate.py.in")
ae += GenerateMayaAE("maya/%sTemplate.py" % rampc3_maya_name, "maya/UserDataRampC3Template.py.in")

if sys.platform != "win32":
   env.Append(CPPFLAGS=" -Wno-unused-parameter")
else:
   env.Append(CPPFLAGS=" /wd4100") # unreferenced format parameter

prjs = [
  {"name": name,
   "type": "dynamicmodule",
   "ext": arnold.PluginExt(),
   "prefix": "arnold",
   "defs": ["PREFIX=\\\"%s\\\"" % prefix],
   "srcs": glob.glob("src/*.cpp"),
   "install": {"arnold": mtd,
               "maya/ae": ae},
   "custom": [arnold.Require]
  }
]

targets = excons.DeclareTargets(env, prjs)

targets[name].extend(mtd)
targets["maya"] = ae

excons.EcosystemDist(env, "user_data_ramp.env", {name: "", "maya": "/maya/ae"}, name=name)

Default([name])
