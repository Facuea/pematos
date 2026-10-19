#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

// Marca de agua: nodo persistente entre escenas, esquina izquierda, z altísimo.
// Se reasegura cada frame (visible, opaco, en escena) para que ningún otro código del juego la oculte.
class Watermark : public CCNode {
	CCLabelBMFont* m_label = nullptr;
public:
	static Watermark* create() {
		auto r = new Watermark();
		if (r && r->init()) { r->autorelease(); return r; }
		delete r; return nullptr;
	}
	bool init() override {
		if (!CCNode::init()) return false;
		m_label = CCLabelBMFont::create("", "bigFont.fnt");
		m_label->setScale(0.35f);
		m_label->setAnchorPoint({0.f, 0.5f});
		m_label->setColor({255, 60, 60});
		this->addChild(m_label);
		this->scheduleUpdate();
		return true;
	}
	void update(float) override {
		int t = Mod::get()->getSettingValue<int64_t>("ticks");
		m_label->setString(("TICK ASSIST ACTIVO (" + std::to_string(t) + " ticks) - NO VALIDO PARA RECORDS").c_str());
		m_label->setPosition({4.f, CCDirector::get()->getWinSize().height / 2});
		m_label->setVisible(true);
		m_label->setOpacity(255);
		this->setVisible(true);
		this->setZOrder(100000);
		auto scene = CCDirector::get()->getRunningScene();
		if (scene && this->getParent() != scene) {
			this->removeFromParentAndCleanup(false);
			scene->addChild(this);
		}
	}
};

$on_mod(Loaded) {
	auto w = Watermark::create();
	SceneManager::get()->keepAcrossScenes(w);
}

class $modify(AssistLayer, GJBaseGameLayer) {
	struct Fields {
		int pending = 0;
		bool held = false;
		bool injecting = false;
	};

	void handleButton(bool down, int button, bool isPlayer1) {
		GJBaseGameLayer::handleButton(down, button, isPlayer1);
		auto f = m_fields.self();
		if (f->injecting || button != 1 || !isPlayer1) return;
		f->held = down;
		int ticks = Mod::get()->getSettingValue<int64_t>("ticks");
		// click temprano en el aire: recordarlo N ticks
		if (down && ticks > 0 && m_player1 && !m_player1->m_isOnGround) f->pending = ticks;
	}

	void processCommands(float dt) {
		GJBaseGameLayer::processCommands(dt);
		auto f = m_fields.self();
		if (f->pending <= 0) return;
		f->pending--;
		if (m_player1 && m_player1->m_isOnGround) {
			f->pending = 0;
			f->injecting = true;
			GJBaseGameLayer::handleButton(false, 1, true);
			GJBaseGameLayer::handleButton(true, 1, true);
			if (!f->held) GJBaseGameLayer::handleButton(false, 1, true);
			f->injecting = false;
		}
	}
};
