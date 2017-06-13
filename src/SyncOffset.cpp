#include "SyncOffset.hpp"

SyncOffset::SyncOffset()
{
	type = SyncOffsetType::None;
}

SyncOffset		SyncOffset::CreateLinearSyncOffset(const int delaySecs, const int delayMillis, const SyncOffsetOrder order)
{
	SyncOffset	o;

	o.type = SyncOffsetType::Linear;
	o.order = order;
	o.linearDelay.tv_sec = delaySecs;
	o.linearDelay.tv_usec = delayMillis * 1000;
	return o;
}

SyncOffset		SyncOffset::CreateCustomSyncOffset(CustomSyncOffsetCallback callback, const SyncOffsetOrder order)
{
	SyncOffset	o;

	o.type = SyncOffsetType::Custom;
	o.order = order;
	bzero(&o.linearDelay, sizeof(Timeval));
	gettimeofday(&o.linearDelay, NULL);
	o.customDelayCallback = callback;
	return o;
}

SyncOffset		SyncOffset::CreateNoneSyncOffset(void)
{
	SyncOffset	o;

	o.type = SyncOffsetType::None;
	return o;
}

SyncOffset::~SyncOffset(void) {}
