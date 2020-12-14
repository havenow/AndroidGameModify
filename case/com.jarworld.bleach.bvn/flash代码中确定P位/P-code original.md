#
```
trait method Qname(PackageNamespace(""),"useEnergy")
dispid 0
method
name "useEnergy"
flag HAS_PARAM_NAMES
param Qname(PackageNamespace(""),"Number")
paramname "v"
returns Qname(PackageNamespace(""),"void")

body
maxstack 3
localcount 2
initscopedepth 0
maxscopedepth 1

code
getlocal_0
pushscope
findpropstrict Qname(PackageNamespace(""),"energy")
dup
getproperty Qname(PackageNamespace(""),"energy")
getlocal_1
subtract
setproperty Qname(PackageNamespace(""),"energy")
pushdouble 0.8
pushbyte 30
multiply
findproperty Qname(PrivateNamespace("net.play5d.game.bvn.fighter:FighterMain"),"_energyAddGap")
swap
setproperty Qname(PrivateNamespace("net.play5d.game.bvn.fighter:FighterMain"),"_energyAddGap")
getlex Qname(PackageNamespace(""),"energy")
pushbyte 0
ifnlt ofs0035
pushbyte 0
findproperty Qname(PackageNamespace(""),"energy")
swap
setproperty Qname(PackageNamespace(""),"energy")
pushtrue
findproperty Qname(PackageNamespace(""),"energyOverLoad")
swap
setproperty Qname(PackageNamespace(""),"energyOverLoad")
ofs0035:returnvoid ; trait
end ; code
end ; body
end ; method
end ; trait

```