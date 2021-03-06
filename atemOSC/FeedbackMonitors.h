#import "BMDSwitcherAPI.h"
#import "VVOSC/VVOSC.h"
#import <list>
#import <vector>

#ifndef SwitcherMonitor_h
#define SwitcherMonitor_h

// sendStatus should return the delay, or how long we think we
// should wait after those send commands to allow time for the
// network queue to flush, to make sure we don't flood the network
// too fast.  Monitors that only send 1-2 commands should only delay
// less than 0.05 seconds, while monitors that sends 10-20 commands
// may need to delay 0.1 - 0.3 seconds
class SendStatusInterface
{
public:
	virtual float sendStatus() const = 0;
};

template <class T=IUnknown>
class GenericMonitor : public T
{
public:
	GenericMonitor(void *delegate) : appDel(delegate), mRefCount(1) { }
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv);
	ULONG STDMETHODCALLTYPE AddRef(void);
	ULONG STDMETHODCALLTYPE Release(void);
	
protected:
	virtual ~GenericMonitor() { }
	void *appDel;
	
private:
	int mRefCount;
};

// Callback class for monitoring property changes on a mix effect block.
class MixEffectBlockMonitor : public GenericMonitor<IBMDSwitcherMixEffectBlockCallback>, public SendStatusInterface
{
public:
	MixEffectBlockMonitor(void *delegate) : GenericMonitor(delegate) { }
	HRESULT PropertyChanged(BMDSwitcherMixEffectBlockPropertyId propertyId);
	bool moveSliderDownwards() const;
	bool mMoveSliderDownwards = false;
	void updateProgramButtonSelection() const;
	void updatePreviewButtonSelection() const;
	void updateInTransitionState();
	void updateSliderPosition();
	float sendStatus() const;
	
protected:
	virtual ~MixEffectBlockMonitor() { }
	
private:
	bool mCurrentTransitionReachedHalfway_ = false;
};

class DownstreamKeyerMonitor : public GenericMonitor<IBMDSwitcherDownstreamKeyCallback>, public SendStatusInterface
{
public:
	DownstreamKeyerMonitor(void *delegate) : GenericMonitor(delegate) { }
	HRESULT Notify (BMDSwitcherDownstreamKeyEventType eventType);
	float sendStatus() const;
	
protected:
	virtual ~DownstreamKeyerMonitor() { }
	
private:
	void updateDSKOnAir() const;
	void updateDSKTie() const;
};

class TransitionParametersMonitor : public GenericMonitor<IBMDSwitcherTransitionParametersCallback>, public SendStatusInterface
{
public:
	TransitionParametersMonitor(void *delegate) : GenericMonitor(delegate) { }
	HRESULT Notify (BMDSwitcherTransitionParametersEventType eventType);
	float sendStatus() const;
	
protected:
	virtual ~TransitionParametersMonitor() { }
	
private:
	void updateTransitionParameters() const;
};

class MacroPoolMonitor : public GenericMonitor<IBMDSwitcherMacroPoolCallback>, public SendStatusInterface
{
public:
	MacroPoolMonitor(void *delegate) : GenericMonitor(delegate) { }
	HRESULT Notify (BMDSwitcherMacroPoolEventType eventType, uint32_t index, IBMDSwitcherTransferMacro* macroTransfer);
	float sendStatus() const;
	
protected:
	virtual ~MacroPoolMonitor() { }
	
private:
	void updateMacroName(int index) const;
	void updateMacroDescription(int index) const;
	void updateNumberOfMacros() const;
	void updateMacroValidity(int index) const;
};

// Callback class to monitor switcher disconnection
class SwitcherMonitor : public GenericMonitor<IBMDSwitcherCallback>, public SendStatusInterface
{
public:
	SwitcherMonitor(void *delegate) : GenericMonitor(delegate) { }
	HRESULT STDMETHODCALLTYPE Notify(BMDSwitcherEventType eventType, BMDSwitcherVideoMode coreVideoMode);
	float sendStatus() const;
	
protected:
	virtual ~SwitcherMonitor() { }
};

// Callback class to monitor audio inputs
class AudioInputMonitor : public GenericMonitor<IBMDSwitcherAudioInputCallback>, public SendStatusInterface
{
public:
	AudioInputMonitor(void *delegate, BMDSwitcherAudioInputId inputId) : GenericMonitor(delegate), inputId_(inputId) { }
	HRESULT STDMETHODCALLTYPE Notify (BMDSwitcherAudioInputEventType eventType);
	HRESULT STDMETHODCALLTYPE LevelNotification (double left, double right, double peakLeft, double peakRight);
	float sendStatus() const;
	
protected:
	virtual ~AudioInputMonitor() { }
	
private:
	void updateGain() const;
	void updateBalance() const;
	BMDSwitcherAudioInputId  inputId_;
};

// Callback class to monitor audio mixer
class AudioMixerMonitor : public GenericMonitor<IBMDSwitcherAudioMixerCallback>, public SendStatusInterface
{
public:
	AudioMixerMonitor(void *delegate) : GenericMonitor(delegate) { }
	HRESULT STDMETHODCALLTYPE Notify (BMDSwitcherAudioMixerEventType eventType);
	HRESULT STDMETHODCALLTYPE ProgramOutLevelNotification (double left, double right, double peakLeft, double peakRight);
	float sendStatus() const;
	
protected:
	virtual ~AudioMixerMonitor() { }
	
private:
	void updateGain() const;
	void updateBalance() const;
};

#endif /* SwitcherMonitor_h */
