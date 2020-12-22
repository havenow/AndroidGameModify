# 设置类的public成员变量
```
public class FighterMain extends BaseGameSprite
   {
		public var qi:Number = 0;
		
as代码：
 qi = 300;
 
pcode:
pushdouble 300.0
findproperty Qname(PackageNamespace(""),"qi")
swap
setproperty Qname(PackageNamespace(""),"qi")
```

#设置类的private成员变量
```
public class FighterMain extends BaseGameSprite
   {
		private var _speedBack:Number = 0;
		
as代码：
 _speedBack = 300;
 
pcode:
pushdouble 300.0
findproperty Qname(PrivateNamespace("net.play5d.game.bvn.fighter:FighterMain"),"_speedBack")
swap
setproperty Qname(PrivateNamespace("net.play5d.game.bvn.fighter:FighterMain"),"_speedBack")
```

# 取类的private成员变量
```
public class BaseGameSprite extends EventDispatcher implements IGameSprite
   {
		private var _team:TeamVO;
		
public class TeamVO
   {
      public var id:int;
   
as代码：
var _loc2_:int = 0;
         _loc2_ = _team.id;
         if(_loc2_ == 1)
         {
            qi = 300;
            _speedBack = 300;
         }
		 
pcode:
		 
pushbyte 0
setlocal 2
getlex Qname(PrivateNamespace("net.play5d.game.bvn.interfaces:BaseGameSprite"),"_team")
getproperty Qname(PackageNamespace(""),"id")
convert_i
setlocal 2
getlocal 2
pushbyte 1
ifne ofs002b
ofs002b:
```

# 在类FighterMain的 renderAnimate 函数中设置_speedBack的为team.id
```
经过查找_speedBack变量没有被使用
renderAnimate一直在调用
team.id的内存没找到确认的方法
存到_speedBack变量中，_speedBack变量和血的地址偏移是0xF4
```
