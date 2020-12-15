# 不掉血 对应的变量 hp，类型是 Number 占用8个字节，可以当成double
```
package net.play5d.game.bvn.interfaces
	public class BaseGameSprite extends EventDispatcher implements IGameSprite
	
	public function loseHp(param1:Number) : void
      {
         if(!isAllowLoseHP)
         {
            return;
         }
         var _loc3_:* = Number(2 - defenseRate);
         if(_loc3_ < 0.1)
         {
            _loc3_ = 0.1;
         }
         if(_loc3_ > 1)
         {
            _loc3_ = 1;
         }
         var _loc2_:Number = param1 * _loc3_ - defense;
         if(_loc2_ < 0)
         {
            return;
         }
         hp = hp - _loc2_;
         if(hp < 0)
         {
            hp = 0;
         }
      }
team.id == 1时，直接赋值hp为1000，在返回；或者直接返回
```

# 不掉蓝	对应的变量 energy，类型是 Number 占用8个字节，可以当成double
```
package net.play5d.game.bvn.fighter
 public class FighterMain extends BaseGameSprite
 
    public function useEnergy(param1:Number) : void
      {
         energy = energy - param1;
         _energyAddGap = 0.8 * 30;
         if(energy < 0)
         {
            energy = 0;
            energyOverLoad = true;
         }
      }
team.id == 1时，直接赋值energy为100，在返回
```

# 无线sp	对应的变量 qi，类型是 Number 占用8个字节，可以当成double
```
package net.play5d.game.bvn.fighter
 public class FighterMain extends BaseGameSprite
 
      public function useQi(param1:Number) : Boolean
      {
         if(qi < param1)
         {
            return false;
         }
         qi = qi - param1;
         return true;
      }
team.id == 1时，直接赋值qi为300，在返回
```

# 无限辅助 对应的变量 fzqi，类型是 Number 占用8个字节，可以当成double
```
package net.play5d.game.bvn.fighter
 public class FighterMain extends BaseGameSprite
 
	private function renderFzQi() : void
      {
         if(fzqi < 100)
         {
            fzqi = fzqi + 0.2;
         }
      }
team.id == 1时，直接赋值fzqi为100，在返回
```