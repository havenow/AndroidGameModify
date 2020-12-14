#
```
package net.play5d.game.bvn.fighter
{
	public class FighterMain extends BaseGameSprite
   {
	  public var qi:Number = 0;
      public const qiMax:Number = 300;
      public var energy:Number = 100;
      public var energyMax:Number = 100;
      public var energyOverLoad:Boolean = false;
      public var customHpMax:int = 0;
      public var fzqi:Number = 100;
      public const fzqiMax:Number = 100;

	  public function useEnergy(param1:Number) : void
      {
         var _loc2_:int = 0;
         _loc2_ = _team.id;
         if(_loc2_ == 0)
         {
            qi = 300;
            return;
         }
         energy = energy - param1;
         _energyAddGap = 0.8 * 30;
         if(energy < 0)
         {
            energy = 0;
            energyOverLoad = true;
         }
      }
	  
	}
}

package net.play5d.game.bvn.interfaces
{
   public class BaseGameSprite extends EventDispatcher implements IGameSprite
   {
      public var isInAir:Boolean;
      public var isTouchBottom:Boolean;
      public var isAllowBeHit:Boolean = true;
      public var isCross:Boolean = false;
      public var isAlive:Boolean = true;
      public var isAllowLoseHP:Boolean = false;
      public var isApplyG:Boolean = true;
      public var heavy:Number = 2;
      public var hp:Number = 1000;
      public var hpMax:Number = 1000;
      	
	  private var _team:TeamVO;		//注意是private
   }
}

P-code 代码获取_team.id
getlex Qname(PrivateNamespace("net.play5d.game.bvn.interfaces:BaseGameSprite"),"_team")
getproperty Qname(PackageNamespace(""),"id")
convert_i
setlocal 2
```