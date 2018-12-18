//
//  MarchingScene.cpp
//  Created by 楊頌聲 on 2016/10/31.
//

#include "marching/MarchingScene.h"

#include "UICommon.h"

#include "battle/BattleSceneData.h"
#include "battle/data/repository/BattleDataRepository.h"

#include "common/system/sound/manager/SoundManager.h"
#include "common/util/memory/LivingPointerPool.h"

#include "home/HomeSceneData.h"

#include "marching/constants/CameraConstants.h"
#include "marching/loading/UnitLoadingLayer.h"
#include "marching/mode/MarchingMode.h"
#include "marching/status/StatusLayer.h"

USING_NS(marching)

// ---------- インスタンスメソッド
// コンストラクタ
MarchingScene::MarchingScene()
{
    CCLOGFUNC

    LivingPointerPool::getInstance()->add(this);
}

// デストラクタ
MarchingScene::~MarchingScene()
{
    CCLOGFUNC

    LivingPointerPool::getInstance()->remove(this);
}

// 初期化
bool MarchingScene::init(MarchingSceneData* data)
{
    if (!CastleScene::init(data)) return false;

    this->data = data;

    this->setColor(Color3B::BLACK);

    MarchingMode* marchingMode { data->getMarchingMode() };
    if (marchingMode->getConfig(MarchingMode::Config::SHOW_LOADING))
    {
        if (marchingMode->getConfig(MarchingMode::Config::GOING_TO_BATTLE))
        {
            UnitLoadingLayer* loadingLayer { UnitLoadingLayer::create(marchingMode, data->getUnitAnimationStartFrame()) };
            loadingLayer->setPosition(pointToParent(loadingLayer, this, HPos::CENTER, VPos::CENTER));
            this->addChild(loadingLayer);

            // マーチング時にSEを鳴らす
            SoundManager::getInstance()->playSE(sound::effect::Type::MARCHING);
        }
        else
        {
            CastleLayerColor* background { CastleLayerColor::create({ 120, 200, 230, 255 }) };
            background->setContentSize(Director::getInstance()->getWinSize());
            background->setPosition(pointToParent(background, this, HPos::CENTER, VPos::CENTER));
            this->addChild(background);

            Sprite* backIcon { Sprite::createWithSpriteFrameName("loading_image_battle.png") };
            backIcon->setPosition(pointToParent(backIcon, background, HPos::CENTER, VPos::CENTER));
            background->addChild(backIcon);
        }
    }

    StatusLayer* statusLayer { StatusLayer::create(marchingMode) };
    statusLayer->setVisible(marchingMode->getConfig(MarchingMode::Config::SHOW_LOADING));
    statusLayer->setPosition(pointToParent(statusLayer, this, HPos::CENTER, VPos::CENTER));
    this->addChild(statusLayer);
    this->statusLayer = statusLayer;

    return true;
}

// 画面表示時
void MarchingScene::onEnter()
{
    CastleScene::onEnter();

    this->setupEnvironment();
}

// シーン開始トランジション終了時
void MarchingScene::onEnterTransitionDidFinish()
{
    CastleScene::onEnterTransitionDidFinish();

    // シーン遷移準備
    this->data->getMarchingMode()->prepareForTransition([this](MarchingMode*, bool success)
    {
        if (!LivingPointerPool::getInstance()->isAlive(this)) return;

        this->statusLayer->setCallback([this, success]
        {
            // NOTICE: すぐに遷移すると正しく遷移出来ないのでrunAction後に呼ぶ (要調査)
            // どうやらFadeTransitionの_inSceneと_outSceneが正しく設定されていないのがダメっぽい？
            this->runAction(CallFunc::create([this, success]
            {
                if (success && this->data->getMarchingMode()->getConfig(MarchingMode::Config::GOING_TO_BATTLE))
                {
                    this->goBattle();
                }
                else
                {
                    this->goBack();
                }
            }));
        });

        this->statusLayer->setContinuable(true);
    });
}

// 3D環境をセットアップ
void MarchingScene::setupEnvironment()
{
    this->getDefaultCamera()->setDepth(ETOI(camera::Depth::UI));
}

// バトルへ遷移
void MarchingScene::goBattle()
{
    BattleSceneData* data { BattleSceneData::create(battle::BattleDataRepository::getInstance()->getBattleMode()) };
    tl::core::SceneManager::getInstance()->replaceScene(data);
}

// 帰還後のシーンへ遷移
void MarchingScene::goBack()
{
    CastleSceneData* sceneData { this->data->getMarchingMode()->returnSceneData };
    if (!sceneData) sceneData = HomeSceneData::create();

    if (HomeSceneData * homeSceneData { dynamic_cast<HomeSceneData*>(sceneData) })
    {
        homeSceneData->setPrevSceneType(scene::Type::MARCHING);
    }

    tl::core::SceneManager::getInstance()->replaceScene(sceneData);
}

// Androidのバックキーがおされた時
void MarchingScene::onAndroidBackKeyTouched()
{
    // 何もしない
}
