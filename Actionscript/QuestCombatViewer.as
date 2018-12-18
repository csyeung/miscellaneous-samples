package com.plasticfantastic.sexyvampires.ui.quests
{

	public class QuestCombatViewer extends CombatViewer
	{
		public function QuestCombatViewer()
		{
			super();
		}
		
		override private function CalculateAttacks(heroIndex : int, enemyIndex: int) : void 
		{
			var _hero: HeroState = this.combat.heroMembers[heroIndex];
			var _enemy: HeroState = this.combat.enemyMembers[enemyIndex];
			
			var currHeroHP:uint = _hero.stats[ "Endurance" ] + _hero.stats[ "Armour" ] * 10;
			var currEnemyHP:uint =  _enemy.stats[ "Endurance" ] + _enemy.stats[ "Armour" ] * 10;
			
			_hero.SetHitPoints(currHeroHP);
			_hero.ResetAttackStats();
			_enemy.SetHitPoints(currEnemyHP);
			_enemy.ResetAttackStats();
			
			var pAttacker:BaseEntityState;
			var pDefender:BaseEntityState;
			
			// 1. Decide who will hit first by comparing the intelligence between the first hero and first monster
			var heroAttacker : Boolean;
			
			if (this.combat.heroAttacksFirst)
			{
				heroAttacker = true;
				pAttacker = _hero;
				pDefender = _enemy;
			}
			else
			{
				heroAttacker = false;
				pAttacker = _enemy;
				pDefender = _hero;
			}
			
			var pTemp:BaseEntityState;
			
			var flag:Boolean = true;
			
			var randGen : RandomGen = new RandomGen();
			randGen.seed = this.combat.attackSeed;
			
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
		}
	}
}