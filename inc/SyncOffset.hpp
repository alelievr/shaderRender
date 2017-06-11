#ifndef SYNCOFFSET_HPP
# define SYNCOFFSET_HPP
# include "Timeval.hpp"
# include <iostream>
# include <string>

typedef std::function< Timeval & (const int seat, const int row, const int index) >		CustomSyncOffsetCallback;

enum class		SyncOffsetType
{
	None,
	Linear,
	Custom,
};

enum class		SyncOffsetOrder
{
	Ascending,
	Descending,
	Random,
	Custom,
};

struct		SyncOffset
{
	SyncOffsetType				type;
	SyncOffsetOrder				order;
	Timeval						linearDelay;
	CustomSyncOffsetCallback	customDelayCallback;

	SyncOffset(void);
	SyncOffset(const SyncOffset &);
	virtual ~SyncOffset(void);

	static SyncOffset			CreateLinearSyncOffset(const int delaySecs, const int delayMillis, const SyncOffsetOrder order = SyncOffsetOrder::Ascending);
	static SyncOffset			CreateCustomSyncOffset(CustomSyncOffsetCallback callback, const SyncOffsetOrder order = SyncOffsetOrder::Ascending);
	static SyncOffset			CreateNoneSyncOffset(void);

	SyncOffset &	operator=(SyncOffset const & src) = delete;
};

#endif
