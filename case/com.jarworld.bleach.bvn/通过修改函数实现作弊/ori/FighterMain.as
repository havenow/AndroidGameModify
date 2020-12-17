package net.play5d.game.bvn.fighter
{
   import flash.display.MovieClip;
   import flash.geom.ColorTransform;
   import flash.geom.Rectangle;
   import net.play5d.game.bvn.ctrl.GameLogic;
   import net.play5d.game.bvn.data.FighterVO;
   import net.play5d.game.bvn.data.TeamVO;
   import net.play5d.game.bvn.fighter.ctrler.FighterBuffCtrler;
   import net.play5d.game.bvn.fighter.ctrler.FighterCtrler;
   import net.play5d.game.bvn.fighter.models.HitVO;
   import net.play5d.game.bvn.interfaces.BaseGameSprite;
   import net.play5d.game.bvn.interfaces.IFighterActionCtrl;
   import net.play5d.game.bvn.interfaces.IGameSprite;
   
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
      
      public var speed:Number = 6;
      
      public var jumpPower:Number = 15;
      
      public var isSteelBody:Boolean = false;
      
      public var isSuperSteelBody:Boolean = false;
      
      public var data:FighterVO;
      
      public var airHitTimes:int = 1;
      
      public var jumpTimes:int = 2;
      
      public var actionState:int = 0;
      
      public var defenseType:int = 0;
      
      public var lastHitVO:HitVO;
      
      private var _buffCtrler:FighterBuffCtrler;
      
      private var _currentHurts:Vector.<HitVO>;
      
      public var hurtHit:HitVO;
      
      public var defenseHit:HitVO;
      
      public var targetTeams:Vector.<TeamVO>;
      
      private var _currentTarget:IGameSprite;
      
      private var _fighterCtrl:FighterCtrler;
      
      private var _energyAddGap:int;
      
      private var _explodeHitVO:HitVO;
      
      private var _explodeHitFrame:int;
      
      private var _explodeSteelFrame:int;
      
      private var _replaceSkillFrame:int;
      
      private var _speedBack:Number = 0;
      
      private var _colorTransform:ColorTransform;
      
      public function FighterMain(param1:MovieClip)
      {
         super(param1);
         _area = null;
      }
      
      public function get colorTransform() : ColorTransform
      {
         return _colorTransform;
      }
      
      public function set colorTransform(param1:ColorTransform) : void
      {
         _colorTransform = param1;
         _mainMc.transform.colorTransform = !!param1?param1:new ColorTransform();
      }
      
      public function changeColor(param1:ColorTransform) : void
      {
         _mainMc.transform.colorTransform = param1;
      }
      
      public function resumeColor() : void
      {
         _mainMc.transform.colorTransform = !!_colorTransform?_colorTransform:new ColorTransform();
      }
      
      override public function destory(param1:Boolean = true) : void
      {
         if(!param1)
         {
            return;
         }
         if(_fighterCtrl)
         {
            _fighterCtrl.destory();
            _fighterCtrl = null;
         }
         if(_mainMc)
         {
            _mainMc.filters = null;
            _mainMc.gotoAndStop(1);
         }
         if(_buffCtrler)
         {
            _buffCtrler.destory();
            _buffCtrler = null;
         }
         targetTeams = null;
         _currentTarget = null;
         _currentHurts = null;
         super.destory(param1);
      }
      
      override public function set attackRate(param1:Number) : void
      {
         .super.attackRate = param1;
         if(_fighterCtrl && _fighterCtrl.hitModel)
         {
            _fighterCtrl.hitModel.setPowerRate(param1);
         }
      }
      
      public function currentHurtDamage() : int
      {
         if(!_currentHurts)
         {
            return 0;
         }
         var _loc1_:int = 0;
         var _loc4_:int = 0;
         var _loc3_:* = _currentHurts;
         for each(var _loc2_ in _currentHurts)
         {
            _loc1_ = _loc1_ + _loc2_.getDamage();
         }
         return _loc1_;
      }
      
      public function getLastHurtHitVO() : HitVO
      {
         if(!_currentHurts)
         {
            return null;
         }
         return _currentHurts[_currentHurts.length - 1];
      }
      
      public function hurtBreakHit() : Boolean
      {
         var _loc3_:int = 0;
         var _loc2_:* = _currentHurts;
         for each(var _loc1_ in _currentHurts)
         {
            if(_loc1_.isBreakDef)
            {
               return true;
            }
         }
         return false;
      }
      
      public function clearHurtHits() : void
      {
         _currentHurts = null;
      }
      
      public function getCtrler() : FighterCtrler
      {
         return _fighterCtrl;
      }
      
      public function getBuffCtrl() : FighterBuffCtrler
      {
         return _buffCtrler;
      }
      
      public function getCurrentTarget() : IGameSprite
      {
         if(_currentTarget)
         {
            if(_currentTarget is BaseGameSprite && (_currentTarget as BaseGameSprite).isAlive)
            {
               return _currentTarget;
            }
         }
         var _loc2_:Vector.<IGameSprite> = getTargets();
         var _loc1_:Array = [];
         if(_loc2_ && _loc2_.length > 0)
         {
            var _loc5_:int = 0;
            var _loc4_:* = _loc2_;
            for each(var _loc3_ in _loc2_)
            {
               if(_loc3_.getBodyArea() == null)
               {
                  _loc1_.push({
                     "fighter":_loc3_,
                     "order":5
                  });
               }
               else if(_loc3_ is FighterMain && (_loc3_ as FighterMain).isAlive)
               {
                  _loc1_.push({
                     "fighter":_loc3_,
                     "order":0
                  });
               }
               else if(_loc3_ is BaseGameSprite && (_loc3_ as BaseGameSprite).isAlive)
               {
                  _loc1_.push({
                     "fighter":_loc3_,
                     "order":1
                  });
               }
               else
               {
                  _loc1_.push({
                     "fighter":_loc3_,
                     "order":2
                  });
               }
            }
            _loc1_.sortOn("order",16);
            _currentTarget = _loc1_[0].fighter;
         }
         return _currentTarget;
      }
      
      public function getTargets() : Vector.<IGameSprite>
      {
         var _loc2_:int = 0;
         if(!targetTeams || targetTeams.length < 1)
         {
            return null;
         }
         var _loc1_:Vector.<IGameSprite> = new Vector.<IGameSprite>();
         while(_loc2_ < targetTeams.length)
         {
            _loc1_ = _loc1_.concat(targetTeams[_loc2_].getAliveChildren());
            _loc2_++;
         }
         return _loc1_;
      }
      
      public function getMC() : FighterMC
      {
         if(!_fighterCtrl)
         {
            return null;
         }
         if(!_fighterCtrl.getMcCtrl())
         {
            return null;
         }
         return _fighterCtrl.getMcCtrl().getFighterMc();
      }
      
      public function setActionCtrl(param1:IFighterActionCtrl) : void
      {
         if(_fighterCtrl)
         {
            _fighterCtrl.setActionCtrl(param1);
            param1.initlize();
         }
      }
      
      public function initlize() : void
      {
         _fighterCtrl = new FighterCtrler();
         _buffCtrler = new FighterBuffCtrler(this);
         _fighterCtrl.initFighter(this);
         _mainMc.gotoAndStop(!!data?data.startFrame + 1:2);
      }
      
      override public function renderAnimate() : void
      {
         super.renderAnimate();
         if(_destoryed)
         {
            return;
         }
         renderEnergy();
         renderFzQi();
         if(_fighterCtrl)
         {
            _fighterCtrl.renderAnimate();
         }
         if(_explodeHitFrame > 0)
         {
            _explodeHitFrame = Number(_explodeHitFrame) - 1;
            if(_explodeHitFrame == 8)
            {
               idle();
               isAllowBeHit = false;
            }
            if(_explodeHitFrame <= 0)
            {
               _explodeHitVO = null;
               isAllowBeHit = true;
            }
         }
         if(_explodeSteelFrame > 0)
         {
            _explodeSteelFrame = Number(_explodeSteelFrame) - 1;
            _fighterCtrl.getMcCtrl().setSteelBody(true,true);
            if(_explodeSteelFrame <= 0)
            {
               _fighterCtrl.getMcCtrl().setSteelBody(false);
            }
         }
         if(_replaceSkillFrame > 0)
         {
            _replaceSkillFrame = Number(_replaceSkillFrame) - 1;
            if(_replaceSkillFrame <= 0)
            {
               isAllowBeHit = true;
            }
         }
      }
      
      override public function render() : void
      {
         super.render();
         if(_destoryed)
         {
            return;
         }
         if(_fighterCtrl)
         {
            _fighterCtrl.render();
         }
         if(_buffCtrler)
         {
            _buffCtrler.render();
         }
         if(hp < 0)
         {
            hp = 0;
         }
         if(hp > hpMax)
         {
            hp = hpMax;
         }
         if(qi < 0)
         {
            qi = 0;
         }
         if(qi > 300)
         {
            qi = 300;
         }
         if(fzqi < 0)
         {
            fzqi = 0;
         }
         if(fzqi > 100)
         {
            fzqi = 100;
         }
      }
      
      public function jump() : void
      {
         _g = 0;
         setVelocity(0,-jumpPower);
         setDamping(0,0.5);
      }
      
      override public function getCurrentHits() : Array
      {
         if(_explodeHitVO && _explodeHitFrame < 8)
         {
            return [_explodeHitVO];
         }
         return _fighterCtrl.getCurrentHits();
      }
      
      override public function getBodyArea() : Rectangle
      {
         if(!_fighterCtrl)
         {
            return null;
         }
         return _fighterCtrl.getBodyArea();
      }
      
      override public function hit(param1:HitVO, param2:IGameSprite) : void
      {
         super.hit(param1,param2);
         lastHitVO = param1;
         var _loc3_:* = 0;
         if(param2 is FighterMain)
         {
            if(param1.isBisha())
            {
               _loc3_ = Number(param1.power * 0);
            }
            else
            {
               _loc3_ = Number(param1.power * 0.17);
            }
            if(_loc3_ > 15)
            {
               _loc3_ = 15;
            }
         }
         addQi(_loc3_);
         GameLogic.hitTarget(param1,this,param2);
      }
      
      override public function beHit(param1:HitVO, param2:Rectangle = null) : void
      {
         if(!isAllowBeHit)
         {
            return;
         }
         super.beHit(param1,param2);
         _fighterCtrl.getMcCtrl().beHit(param1,param2);
         var _loc3_:* = Number(param1.power * 0.08);
         if(_loc3_ > 20)
         {
            _loc3_ = 20;
         }
         addQi(_loc3_);
         if(actionState == 21 || actionState == 22)
         {
            if(!_currentHurts)
            {
               _currentHurts = new Vector.<HitVO>();
            }
            _currentHurts.push(param1);
         }
      }
      
      private function renderEnergy() : void
      {
         if(_energyAddGap > 0)
         {
            _energyAddGap = Number(_energyAddGap) - 1;
            return;
         }
         if(energy < energyMax)
         {
            if(energyOverLoad)
            {
               energy = energy + 0.6;
               if(energy > 30)
               {
                  energyOverLoad = false;
               }
            }
            else if(actionState == 20)
            {
               energy = energy + 0.8;
            }
            else if(FighterActionState.isAttacking(actionState))
            {
               energy = energy + 1.1;
            }
            else
            {
               energy = energy + 2;
            }
         }
      }
      
      private function renderFzQi() : void
      {
         if(fzqi < 100)
         {
            fzqi = fzqi + 0.2;
         }
      }
      
      public function hasEnergy(param1:Number, param2:Boolean = false) : Boolean
      {
         if(energy >= param1)
         {
            return true;
         }
         if(param2)
         {
            if(!energyOverLoad)
            {
               return true;
            }
         }
         return false;
      }
      
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
      
      public function useQi(param1:Number) : Boolean
      {
         if(qi < param1)
         {
            return false;
         }
         qi = qi - param1;
         return true;
      }
      
      public function addQi(param1:Number) : void
      {
         qi = qi + param1;
         if(qi > 300)
         {
            qi = 300;
         }
      }
      
      public function sayIntro() : void
      {
         _fighterCtrl.getMcCtrl().sayIntro();
      }
      
      public function win() : void
      {
         _fighterCtrl.getMcCtrl().doWin();
      }
      
      public function idle() : void
      {
         _fighterCtrl.getMcCtrl().idle();
      }
      
      public function lose() : void
      {
         _fighterCtrl.getMcCtrl().doLose();
      }
      
      public function getHitRange(param1:String) : Rectangle
      {
         return _fighterCtrl.getHitRange(param1);
      }
      
      public function energyExplode() : void
      {
         _fighterCtrl.getEffectCtrl().energyExplode();
         _fighterCtrl.getMcCtrl().setSteelBody(true,true);
         _explodeHitVO = new HitVO();
         var _loc1_:Rectangle = new Rectangle(-100,-200,200,210);
         _explodeHitVO.currentArea = _fighterCtrl.getCurrentRect(_loc1_);
         _explodeHitVO.power = 50;
         _explodeHitVO.hitx = 15 * direct;
         _explodeHitVO.hitType = 5;
         _explodeHitVO.hurtType = 1;
         _explodeHitFrame = 10;
         _explodeSteelFrame = 60;
         isAllowBeHit = false;
      }
      
      public function replaceSkill() : void
      {
         _fighterCtrl.getEffectCtrl().replaceSkill();
         move(250 * direct);
         idle();
         isAllowBeHit = false;
         super.render();
         renderAnimate();
         _fighterCtrl.setDirectToTarget();
         _replaceSkillFrame = 30;
      }
      
      override public function getArea() : Rectangle
      {
         if(!_area)
         {
            _area = getBodyArea();
         }
         return _area;
      }
      
      public function hasWankai() : Boolean
      {
         return _fighterCtrl.getMcCtrl().getFighterMc().checkFrame("万解");
      }
      
      public function die() : void
      {
         hp = 0;
         isAlive = false;
         if(!FighterActionState.isHurting(actionState) && actionState != 30)
         {
            _fighterCtrl.getMcCtrl().getFighterMc().playHurtDown();
         }
      }
      
      public function relive() : void
      {
         isAlive = true;
         idle();
      }
   }
}
