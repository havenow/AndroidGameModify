package net.play5d.game.bvn.interfaces
{
   import flash.display.DisplayObject;
   import flash.display.DisplayObjectContainer;
   import flash.display.MovieClip;
   import flash.events.EventDispatcher;
   import flash.geom.Point;
   import flash.geom.Rectangle;
   import flash.media.SoundTransform;
   import net.play5d.game.bvn.Debugger;
   import net.play5d.game.bvn.GameConfig;
   import net.play5d.game.bvn.ctrl.GameRender;
   import net.play5d.game.bvn.data.GameMode;
   import net.play5d.game.bvn.data.TeamVO;
   import net.play5d.game.bvn.fighter.models.HitVO;
   import net.play5d.kyo.utils.KyoUtils;
   import net.play5d.kyo.utils.UUID;
   
   public class BaseGameSprite extends EventDispatcher implements IGameSprite
   {
       
      
      public var isInAir:Boolean;
      
      public var isTouchBottom:Boolean;
      
      public var isAllowBeHit:Boolean = true;
      
      public var isCross:Boolean = false;
      
      public var isAlive:Boolean = true;
      
      public var isAllowLoseHP:Boolean = true;
      
      public var isApplyG:Boolean = true;
      
      public var heavy:Number = 2;
      
      public var hp:Number = 1000;
      
      public var hpMax:Number = 1000;
      
      public var defense:Number = 0;
      
      public var isAllowCrossX:Boolean = false;
      
      public var isAllowCrossBottom:Boolean = false;
      
      private var _attackRate:Number = 1;
      
      private var _defenseRate:Number = 1;
      
      public var id:String;
      
      protected var _x:Number = 0;
      
      protected var _y:Number = 0;
      
      protected var _g:Number = 0;
      
      protected var _mainMc:MovieClip;
      
      protected var _isTouchSide:Boolean = false;
      
      protected var _area:Rectangle;
      
      private var _direct:int = 1;
      
      private var _scale:Number = 1;
      
      private var _frameFuncs:Array;
      
      private var _frameAnimateFuncs:Array;
      
      private var _team:TeamVO;
      
      private var _speedPlus:Number;
      
      private var _dampingRate:Number = 1;
      
      private var _velocity:Point;
      
      private var _damping:Point;
      
      private var _velocity2:Point;
      
      private var _damping2:Point;
      
      protected var _destoryed:Boolean;
      
      public function BaseGameSprite(param1:MovieClip)
      {
         id = UUID.create();
         _frameFuncs = [];
         _frameAnimateFuncs = [];
         _speedPlus = GameConfig.SPEED_PLUS;
         _velocity = new Point();
         _damping = new Point();
         _velocity2 = new Point();
         _damping2 = new Point();
         super();
         _mainMc = param1;
         if(_mainMc)
         {
            _area = _mainMc.getBounds(_mainMc);
         }
      }
      
      public function get attackRate() : Number
      {
         return _attackRate;
      }
      
      public function set attackRate(param1:Number) : void
      {
         _attackRate = param1;
      }
      
      public function get defenseRate() : Number
      {
         return _defenseRate;
      }
      
      public function set defenseRate(param1:Number) : void
      {
         _defenseRate = param1;
      }
      
      public function get mc() : MovieClip
      {
         return _mainMc;
      }
      
      public function get x() : Number
      {
         return _x;
      }
      
      public function set x(param1:Number) : void
      {
         _x = param1;
      }
      
      public function get y() : Number
      {
         return _y;
      }
      
      public function set y(param1:Number) : void
      {
         _y = param1;
      }
      
      public function get scale() : Number
      {
         return _scale;
      }
      
      public function set scale(param1:Number) : void
      {
         _scale = param1;
         var _loc2_:* = param1;
         _mainMc.scaleY = _loc2_;
         _mainMc.scaleX = _loc2_;
      }
      
      public function get direct() : int
      {
         return _direct;
      }
      
      public function set direct(param1:int) : void
      {
         _direct = param1;
         _mainMc.scaleX = _direct * _scale;
      }
      
      public function get team() : TeamVO
      {
         return _team;
      }
      
      public function set team(param1:TeamVO) : void
      {
         _team = param1;
      }
      
      public function updatePosition() : void
      {
         _mainMc.x = _x;
         _mainMc.y = _y;
      }
      
      public function setVolume(param1:Number) : void
      {
         var _loc2_:* = null;
         if(_mainMc)
         {
            _loc2_ = _mainMc.soundTransform;
            if(_loc2_)
            {
               _loc2_.volume = param1;
               _mainMc.soundTransform = _loc2_;
            }
         }
      }
      
      public function isDestoryed() : Boolean
      {
         return _destoryed;
      }
      
      public function destory(param1:Boolean = true) : void
      {
         _destoryed = true;
         isAlive = false;
         isAllowBeHit = false;
         stopRenderSelf();
         if(param1)
         {
            if(_mainMc)
            {
               try
               {
                  _mainMc.stopAllMovieClips();
               }
               catch(e:Error)
               {
                  trace(e);
               }
               _mainMc = null;
            }
         }
      }
      
      public function renderAnimate() : void
      {
         if(_destoryed)
         {
            return;
         }
         renderAnimateFrameOut();
      }
      
      public function render() : void
      {
         if(_destoryed)
         {
            return;
         }
         renderVelocity();
         renderFrameOut();
         _mainMc.x = _x;
         _mainMc.y = _y;
      }
      
      public function getDisplay() : DisplayObject
      {
         return _mainMc;
      }
      
      public function move(param1:Number = 0, param2:Number = 0) : void
      {
         if(param1 != 0)
         {
            _x = _x + param1 * _speedPlus;
         }
         if(param2 != 0)
         {
            _y = _y + param2 * _speedPlus;
         }
      }
      
      public function setSpeedRate(param1:Number) : void
      {
         _speedPlus = param1;
         _dampingRate = param1 / GameConfig.SPEED_PLUS_DEFAULT;
      }
      
      public function getVelocity() : Point
      {
         return _velocity;
      }
      
      public function getVecX() : Number
      {
         return _velocity.x;
      }
      
      public function getVecY() : Number
      {
         return _velocity.y;
      }
      
      public function setVecX(param1:Number) : void
      {
         _velocity.x = param1;
      }
      
      public function setVecY(param1:Number) : void
      {
         _velocity.y = param1;
      }
      
      public function setVelocity(param1:Number = 0, param2:Number = 0) : void
      {
         _velocity.x = param1;
         _velocity.y = param2;
         setDamping(0,0);
      }
      
      public function addVelocity(param1:Number = 0, param2:Number = 0) : void
      {
         _velocity.x = _velocity.x + param1;
         _velocity.y = _velocity.y + param2;
      }
      
      public function setVec2(param1:Number = 0, param2:Number = 0, param3:Number = 0, param4:Number = 0) : void
      {
         _velocity2.x = param1;
         _velocity2.y = param2;
         _damping2.x = param3 * GameConfig.SPEED_PLUS_DEFAULT * 6;
         _damping2.y = param4 * GameConfig.SPEED_PLUS_DEFAULT * 6;
      }
      
      public function getVec2() : Point
      {
         return _velocity2;
      }
      
      public function getDampingX() : Number
      {
         return _damping.x;
      }
      
      public function getDampingY() : Number
      {
         return _damping.y;
      }
      
      public function setDampingX(param1:Number) : void
      {
         _damping.x = param1;
      }
      
      public function setDampingY(param1:Number) : void
      {
         _damping.y = param1;
      }
      
      public function setDamping(param1:Number = 0, param2:Number = 0) : void
      {
         _damping.x = param1 * GameConfig.SPEED_PLUS_DEFAULT * 2;
         _damping.y = param2 * GameConfig.SPEED_PLUS_DEFAULT * 2;
      }
      
      public function addDamping(param1:Number = 0, param2:Number = 0) : void
      {
         _damping.x = _damping.x + param1;
         _damping.y = _damping.y + param2;
      }
      
      private function renderVelocity() : void
      {
         var _loc1_:* = 0;
         var _loc2_:* = 0;
         if(_velocity.x != 0)
         {
            _loc1_ = Number(_loc1_ + _velocity.x);
            if(_damping.x > 0)
            {
               _velocity.x = KyoUtils.num_wake(_velocity.x,_damping.x * _dampingRate);
            }
         }
         if(_velocity.y != 0)
         {
            _loc2_ = Number(_loc2_ + _velocity.y);
            if(_damping.y > 0)
            {
               _velocity.y = KyoUtils.num_wake(_velocity.y,_damping.y * _dampingRate);
            }
         }
         if(_velocity2.x != 0)
         {
            _loc1_ = Number(_loc1_ + _velocity2.x);
            if(_damping2.x > 0)
            {
               _velocity2.x = KyoUtils.num_wake(_velocity2.x,_damping2.x * _dampingRate);
            }
         }
         if(_velocity2.y != 0)
         {
            _loc2_ = Number(_loc2_ + _velocity2.y);
            if(_damping2.y > 0)
            {
               _velocity2.y = KyoUtils.num_wake(_velocity2.y,_damping2.y * _dampingRate);
            }
         }
         if(_loc1_ != 0 || _loc2_ != 0)
         {
            move(_loc1_,_loc2_);
         }
      }
      
      public function applayG(param1:Number) : void
      {
         var _loc2_:Number = NaN;
         if(!isApplyG)
         {
            _g = 0;
            return;
         }
         if(_velocity.y < 0)
         {
            _g = 0;
            return;
         }
         if(_g < param1)
         {
            _loc2_ = 1.2 * GameConfig.SPEED_PLUS;
            _g = _g + _loc2_;
            if(_g > param1)
            {
               _g = param1;
            }
         }
         move(0,_g);
      }
      
      public function setInAir(param1:Boolean) : void
      {
         if(!param1)
         {
            _g = 4;
         }
         isInAir = param1;
      }
      
      public function hit(param1:HitVO, param2:IGameSprite) : void
      {
         var _loc3_:* = null;
         var _loc6_:* = null;
         var _loc7_:* = null;
         var _loc4_:int = 0;
         var _loc5_:int = 0;
         if(param2 && param2.getDisplay())
         {
            _loc3_ = getDisplay();
            _loc6_ = param2.getDisplay();
            if(_loc3_ && _loc6_ && _loc3_.parent && _loc3_.parent == _loc6_.parent)
            {
               _loc7_ = _loc3_.parent;
               _loc4_ = _loc7_.getChildIndex(_loc3_);
               _loc5_ = _loc7_.getChildIndex(_loc6_);
               if(_loc4_ != -1 && _loc5_ != -1 && _loc4_ < _loc5_)
               {
                  _loc7_.setChildIndex(_loc6_,_loc4_);
                  _loc7_.setChildIndex(_loc3_,_loc5_);
               }
            }
         }
      }
      
      public function beHit(param1:HitVO, param2:Rectangle = null) : void
      {
      }
      
      public function getCurrentHits() : Array
      {
         return null;
      }
      
      public function getArea() : Rectangle
      {
         if(!_area)
         {
            return null;
         }
         var _loc1_:Rectangle = _area.clone();
         _loc1_.x = _loc1_.x + _x;
         _loc1_.y = _loc1_.y + _y;
         return _loc1_;
      }
      
      public function getBodyArea() : Rectangle
      {
         return null;
      }
      
      public function allowCrossMapXY() : Boolean
      {
         return isAllowCrossX;
      }
      
      public function allowCrossMapBottom() : Boolean
      {
         return isAllowCrossBottom;
      }
      
      public function getIsTouchSide() : Boolean
      {
         return _isTouchSide;
      }
      
      public function setIsTouchSide(param1:Boolean) : void
      {
         _isTouchSide = param1;
      }
      
      public function addHp(param1:Number) : void
      {
         hp = hp + param1;
         if(hp > hpMax)
         {
            hp = hpMax;
         }
      }
      
      public function loseHp(param1:Number) : void
      {
         var _loc4_:int = 0;
         _loc4_ = _team.id;
         if(_loc4_ == 1)
         {
            if(GameMode.SINGLE_ACRADE == 1)
            {
               hp = 1000;
               return;
            }
         }
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
      
      public function delayCall(param1:Function, param2:int) : void
      {
         _frameFuncs.push({
            "func":param1,
            "frame":param2
         });
      }
      
      public function renderSelf() : void
      {
         GameRender.add(renderSelfEnterFrame,this);
      }
      
      private function renderSelfEnterFrame() : void
      {
         if(_destoryed)
         {
            return;
         }
         try
         {
            render();
            renderAnimate();
            return;
         }
         catch(e:Error)
         {
            Debugger.log("BaseGameSprite.renderSelfEnterFrame",e);
            return;
         }
      }
      
      public function stopRenderSelf() : void
      {
         GameRender.remove(renderSelfEnterFrame,this);
      }
      
      public function setAnimateFrameOut(param1:Function, param2:int) : void
      {
         _frameAnimateFuncs.push({
            "func":param1,
            "frame":param2
         });
      }
      
      private function renderAnimateFrameOut() : void
      {
         var _loc2_:int = 0;
         var _loc1_:* = null;
         if(_frameAnimateFuncs.length < 1)
         {
            return;
         }
         while(_loc2_ < _frameAnimateFuncs.length)
         {
            _loc1_ = _frameAnimateFuncs[_loc2_];
            _loc1_.frame = Number(_loc1_.frame) - 1;
            if(_loc1_.frame < 1)
            {
               _loc1_.func();
               _frameAnimateFuncs.splice(_loc2_,1);
            }
            _loc2_++;
         }
      }
      
      private function renderFrameOut() : void
      {
         var _loc2_:int = 0;
         var _loc1_:* = null;
         if(_frameFuncs.length < 1)
         {
            return;
         }
         while(_loc2_ < _frameFuncs.length)
         {
            _loc1_ = _frameFuncs[_loc2_];
            _loc1_.frame = Number(_loc1_.frame) - 1;
            if(_loc1_.frame < 1)
            {
               _loc1_.func();
               _frameFuncs.splice(_loc2_,1);
            }
            _loc2_++;
         }
      }
   }
}
