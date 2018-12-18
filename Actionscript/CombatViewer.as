package com.plasticfantastic.sexyvampires.ui.quests
{
	import com.funatics.Shared.Tools.RandomGen;
	import com.greensock.TweenMax;
	import com.greensock.easing.Elastic;
	import com.liquid.controls.LiquidButton;
	import com.plasticfantastic.sexyvampires.game.BaseEntityState;
	import com.plasticfantastic.sexyvampires.game.HeroState;
	import com.plasticfantastic.sexyvampires.game.MonsterState;
	import com.plasticfantastic.sexyvampires.game.SEXYVAMPIRES;
	import com.plasticfantastic.sexyvampires.ui.FadingWindow;
	
	import fl.events.ComponentEvent;
	
	import flash.display.Loader;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.net.URLRequest;
	import flash.text.TextField;
	import flash.text.TextFieldAutoSize;
	
	public class CombatViewer extends FadingWindow
	{
		private const COMBAT_END_LOGO_TIME : Number = 1;
		
		private var healthPanel : CombatHealthPanel = new CombatHealthPanel();
		private var heroTeamPanel : CombatHeroTeamPanel;
		private var enemyTeamPanel : CombatMonsterTeamPanel;
		private var buttonSkip : LiquidButton;
		
		private var resultsTextArea :TextField = new TextField();
		
		private var combat : CombatContainer = new CombatContainer();
		private var combatTurn : int = 0;
		private var combatRound : int = 0;
		private var currentHero : int = 0;
		private var currentMonster : int = 0;
				
		public function CombatViewer(fadeTime:Number=0.5)
		{
			super(SEXYVAMPIRES.ASSETPATH + "ui/Quest/CombatViewer/background.jpg", fadeTime);
		}
		
		override protected function InitAsset(event : Event) : void
		{
			super.InitAsset(event);
			
			// health bar panel
			this.healthPanel.x = this.width / 2 - this.healthPanel.width / 2;
			this.healthPanel.y = 30;
			
			this.addChild(this.healthPanel);
			
			// skip combat button
			buttonSkip = new LiquidButton();
			
			buttonSkip.label = "Skip Combat";
			buttonSkip.width = 100;
			buttonSkip.height = 40;
			buttonSkip.x = this.width / 2 - buttonSkip.width / 2;
			buttonSkip.y = this.healthPanel.y - 3;
			buttonSkip.addEventListener(ComponentEvent.BUTTON_DOWN, OnSkipCombat);
			
			this.addChild(buttonSkip);
			
			this.heroTeamPanel = new CombatHeroTeamPanel(this.combat.heroMembers, this.healthPanel);
			this.heroTeamPanel.x = 15;
			this.heroTeamPanel.y = this.healthPanel.y;
			
			this.addChild(this.heroTeamPanel);
			
			this.enemyTeamPanel = new CombatMonsterTeamPanel(this.combat.monsterMembers, this.healthPanel);
			this.enemyTeamPanel.x = this.width / 2 + 40;
			this.enemyTeamPanel.y = this.healthPanel.y + 200;
			
			this.addChild(this.enemyTeamPanel);
			
			// this.currentHero
			
			// hack to delay 2 seconds
			var sprite : Sprite = new Sprite();
			sprite.alpha = 1;
			
			TweenMax.to(sprite, 1.4, {alpha:0,
									  onComplete:StartCombat});			
		}
		
		public function SetupCombatEnvironment(combatInit : CombatContainer) : void
		{
			this.combat.heroAttacksFirst = combatInit.heroAttacksFirst;
			this.combat.heroMembers = combatInit.heroMembers;
			this.combat.monsterMembers = combatInit.monsterMembers;
			this.combat.attackSeed = combatInit.attackSeed;
//			this.combat.heroAttack = combatInit.heroAttack;
//			this.combat.monsterAttack = combatInit.monsterAttack;
			this.combat.rewardArr = combatInit.rewardArr;
			this.combat.heroWins = combatInit.heroWins;
			
			// BUGBUG: Calculate attacks locally for now as well  Data is already in heroAttack, MonsterAttack
			// need way to show critical hit as well
			this.currentHero = 0;
			this.currentMonster = 0;
			CalculateAttacks(this.currentHero, this.currentMonster);
			
			
		}
		
		private function CalculateAttacks(heroIndex : int, enemyIndex: int) : void 
		{
			var hero : HeroState = this.combat.heroMembers[heroIndex];
			var _monster : MonsterState = this.combat.monsterMembers[enemyIndex];
			
			var currHeroHP:uint = hero.stats[ "Endurance" ] + hero.stats[ "Armour" ] * 10;
			var currMonsterHP:uint = _monster.stats[ "Endurance" ]+ 1 * 10; // armour value for monster is 1 for testing
			
			hero.SetHitPoints(currHeroHP);
			hero.ResetAttackStats();
			_monster.SetHitPoints(currMonsterHP);
			_monster.ResetAttackStats();
			
			var pAttacker:BaseEntityState;// = new BaseEntityState();
			var pDefender:BaseEntityState;// = new BaseEntityState();
			
			//			heroFirstAttack = (pHeroData->GetIntelligence() > pMonsterData->GetIntelligence());
			
			// 1. Decide who will hit first by comparing the intelligence between the first hero and first monster
			var heroAttacker : Boolean;
			
			if (this.combat.heroAttacksFirst)
			{
				heroAttacker = true;
				pAttacker = hero;
				pDefender = _monster;
			}
			else
			{
				heroAttacker = false;
				pAttacker = _monster;
				pDefender = hero;
			}
			
			var pTemp:BaseEntityState;// = new BaseEntityState();
			
			var flag:Boolean = true;
			
			
			var randGen : RandomGen = new RandomGen();
			
			/**
			 * Jonathan: Server side and client side keeps the same random generate functions
			 * And now using the same mechanism to calculate 
			 * 
			 * TODO: Review message structure, checking when doing mission/ quest
			 * 
			 * 12-Aug-2011 
			 * - Modified related parts to make this feature works only
			 */ 
			randGen.seed = this.combat.attackSeed;
			//var date : Date = new Date();
			//randGen.seed = uint(date.getSeconds() * date.getMilliseconds() * date.getMinutes());
			
			// 2. Calculate 
			// CraigCo:  while (1) a little dangerous - should throw a variable in there
			while (flag)
			{
				DoAttack(pAttacker, pDefender, randGen);
				
				if (pDefender.GetHitPoints() <= 0)
				{
					flag = false;
				}
				
				if (flag) 
				{
					// swap attacker and defender
					pTemp = pAttacker;
					pAttacker = pDefender;
					pDefender = pTemp;
					
					pTemp = null;
					
					heroAttacker = !heroAttacker;
					
				}
			}
			
			// win lose determined by if player is attacker -> player won
			//			heroWon = pDefender->GetType() == ENTITYTYPE_MONSTER;
			
			// BUGBUG: M2 for critical hit and battle variation
			//this.combat.heroAttackWithCriticalHit[heroIndex].length = 0;
			//this.combat.monsterAttackWithCriticalHit.length = 0;
			
			if (heroAttacker)
			{
				this.combat.heroAttackWithCriticalHit[heroIndex] = pAttacker.attackVector;
				this.combat.monsterAttackWithCriticalHit = pDefender.attackVector;
			}
			else
			{
				this.combat.heroAttackWithCriticalHit[heroIndex] = pDefender.attackVector;
				this.combat.monsterAttackWithCriticalHit = pAttacker.attackVector;
			}
			
			this.combat.heroWins = heroAttacker;
			//trace(heroAttacker);
			//trace(pAttacker.attackVector);
			//trace(pDefender.attackVector);
		}
		
		private var scaleFactor:uint = 1;
		
		private function DoAttack( pAttacker : BaseEntityState, pDefender : BaseEntityState, randGen : RandomGen): void 
		{
			var randMax : uint = 100;
			
			
			
			// a. Calculate chance to hit
			var isHit : Boolean = true;
			
			var damageStatValue : Number = pAttacker.GetDamageStatValue();
			
			var attackerLuck : Number = pAttacker.stats[ "Luck" ];//->GetLuck();
			var defenderLuck : Number = pDefender.stats[ "Luck" ];//->GetLuck();
			
			var defenderDexterity : Number = pDefender.stats[ "Dexterity" ];
			
			//FunLand::U32 defenderDexterity = pDefender->GetDexterity();
			
			var chanceToHit : Number = Number(damageStatValue + attackerLuck) / Number( defenderDexterity + defenderLuck );
			//FunLand::F32 chanceToHit = FunLand::F32(damageStatValue + attackerLuck) / FunLand::F32( defenderDexterity + defenderLuck );
			
			//FunLand::U32 randNum = GetRandomNumber( randMax );
			var randNum : uint = randGen.nextIntRange( 0, randMax );
			
			isHit = ( randNum < randMax * chanceToHit );
			
			// Disable chance to hit first here, to perform the checking behind
			//hitMiss = true;
			
			var hitPoint : Number = 0;
			var isCritical : Boolean = false;
			
			if ( isHit ) 
			{
				// b. Calculate Hit Point
				// b.1 Randomize a value of the weapon range		
				hitPoint = (pAttacker.GetWeapon() + damageStatValue - pDefender.GetArmor()) * scaleFactor;
				
				// c. Chance to critical hit and hit bonus
				//FunLand::F32 chanceCritical = FunLand::F32( pAttacker->GetLuck() + pAttacker->GetDamageStatValue() * 2 );
				var chanceCritical:Number = Number( pAttacker.stats[ "Luck" ] + pAttacker.GetDamageStatValue() * 2 );
				
				//FunLand::U32 randCritical = GetRandomNumber( randMax );
				var randCritical : uint = randGen.nextIntRange( 0, randMax );
				
				if ( randCritical < chanceCritical )
				{
					isCritical = true;
					
					var bonusHitPoint : Number = Number( hitPoint * ( damageStatValue + pAttacker.stats[ "Luck" ] ) / 100.0 );
					
					hitPoint +=  bonusHitPoint;
				}
			}
			
			var finalHitPoint : uint = uint(Math.round(hitPoint));
				
			// d. Deduct defender's HP
			pDefender.ReduceHitPoints( finalHitPoint );
			
			// e. Push to hero hit stack
			pAttacker.AddAttackDamage( finalHitPoint );
			
			pAttacker.SaveAttackStats(finalHitPoint, isCritical);
		}
		
		private function OnSkipCombat(event : ComponentEvent) : void
		{
			TweenMax.killAll();
			
			EndCombat();
		}
		
		private function StartCombat() : void
		{
			var attackString : String;
			var tokenIndex : int;
			var attackHitpoint : String;
			var attackCriticalHit : String;
			var critical : Boolean = false;
			
			if (!this.combat.heroAttacksFirst) 
			{
				if (this.combatTurn >= this.combat.monsterAttackWithCriticalHit.length)
				{
					EndCombat();
					
					return;
				}
				
				// need way to communicate critical hit
				attackString = this.combat.monsterAttackWithCriticalHit[combatTurn];
				
				tokenIndex = attackString.indexOf(",");
				attackHitpoint = attackString.slice(0, tokenIndex);
				attackCriticalHit = attackString.slice(tokenIndex + 1);
				
				if (attackCriticalHit == "true")
				{
					critical = true;
				}
				
				this.heroTeamPanel.HeroAttacked(int(attackHitpoint), critical);
				
				this.combat.heroAttacksFirst = true;
			} 
			else 
			{
				if (this.combatTurn >= this.combat.heroAttackWithCriticalHit[this.currentHero].length)
				{
					EndCombat();
					
					return;
				}
				
				/*monsterAttackPanel = new Sprite();
				monsterText = new TextField();
				monsterText.text = String(heroAttack[turn]);
				monsterText.autoSize = TextFieldAutoSize.LEFT;
				monsterText.setTextFormat( txtFormat );
				
				monsterAttackPanel.x = 600;
				monsterAttackPanel.y = 200;
				monsterAttackPanel.addChild( monsterText );
				addChild( monsterAttackPanel );*/
				
				// need way to communicate critical hit
				attackString = this.combat.heroAttackWithCriticalHit[currentHero][combatTurn];
				
				tokenIndex = attackString.indexOf(",");
				attackHitpoint = attackString.slice(0, tokenIndex);
				attackCriticalHit = attackString.slice(tokenIndex + 1);
				
				if (attackCriticalHit == "true")
				{
					critical = true;
				}
				
				this.enemyTeamPanel.MonsterAttacked(int(attackHitpoint), critical);
				
				/*monsterHPBarText.text = monsterHP+ "/" + monsterHPbase;
				newWidth = HPBAR_WIDTH * (monsterHP/ monsterHPbase);
				
				if (newWidth< 0)
					newWidth = 0;
				
				TweenMax.to(monsterHPBar, 2, {width: newWidth, ease:Back.easeIn});
				TweenMax.to(monsterAttackPanel, 2 , {alpha:0, y:400, onCompleteListener:stopTextEffect});*/
				
				this.combat.heroAttacksFirst = false;
			}
			
			this.combatRound++;
			
			if (this.combatRound % 2 == 0) 
			{
				this.combatTurn++;
			}
			
			// wait 1.5 seconds and call this function again - HACK
			var sprite : Sprite = new Sprite();
			TweenMax.to(sprite, 2, {alpha:0,
									onComplete:StartCombat});
		}
		
		private var loaderEnd:Loader = new Loader();
		
		private function EndCombat() : void
		{
			
			this.buttonSkip.enabled = false;
			
			var path : String;
			
			if (this.combat.heroWins)
			{
				path = "YouWin.png";
			}
			else
			{
				path = "YouLose.png"
			}
			
			this.loaderEnd.load(new URLRequest(SEXYVAMPIRES.ASSETPATH + "ui/Quest/CombatViewer/" + path));
			this.loaderEnd.contentLoaderInfo.addEventListener(Event.INIT, DoCombatEndScreen);
		}
	
		private function DoCombatEndScreen(event : Event) : void
		{
			this.loaderEnd.x = 512;
			this.loaderEnd.y = 384 - (this.loaderEnd.height / 2) - 200;
			this.loaderEnd.scaleX = 0;
			this.loaderEnd.scaleY = 0;
			this.loaderEnd.rotation = 15;
			
			var sprite : Sprite = new Sprite();
			sprite.graphics.beginFill(0x000000);
			sprite.graphics.drawRect(0, 0, 1024, 768);
			sprite.graphics.endFill();
			sprite.alpha = 0.8;
			this.addChild(sprite);
			
			this.addChild(this.loaderEnd);
			
			if (this.combat.heroWins)
			{
				var backingSprite : Sprite = new Sprite();
				backingSprite.graphics.beginFill(0x000000);
				backingSprite.graphics.drawRect(0, 0, 60, 70);
				backingSprite.graphics.endFill();
				backingSprite.x = 490;
				backingSprite.y = 384;

				this.addChild(backingSprite);
				
				this.resultsTextArea.borderColor = 0xFFFFFF;
				this.resultsTextArea.autoSize = TextFieldAutoSize.LEFT;
				this.resultsTextArea.multiline = true;
				this.resultsTextArea.textColor = 0xFFFFFF;
				this.resultsTextArea.border = true;
				
				this.resultsTextArea.text = "Cash: " + this.combat.rewardArr[0] + "\n"; 
				this.resultsTextArea.appendText("XP: " + this.combat.rewardArr[1] + "\n"); 
				this.resultsTextArea.appendText("Item: " + this.combat.rewardArr[2] + "\n"); 
				this.resultsTextArea.appendText("Message: " + this.combat.rewardArr[3] + "\n");
				this.resultsTextArea.x = 490;
				this.resultsTextArea.y = 384;
				
				this.addChild(this.resultsTextArea);
			}
			
			TweenMax.to(this.loaderEnd, COMBAT_END_LOGO_TIME, {transformAroundCenter:{scaleX:1, scaleY:1, rotation:0}, 
															   ease:Elastic.easeOut,
															   onComplete:ExitCombatViewer});
		}
		
		private function ExitCombatViewer() : void
		{
			var sprite : Sprite = new Sprite();
			
			TweenMax.to(sprite, 2.5, {alpha:0,
									  onComplete:Close});
		}
	}
}