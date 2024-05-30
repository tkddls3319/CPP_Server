#pragma once


class SendBufferChunk;

/*--------------
	SendBuffer
----------------*/
class SendBuffer : public enable_shared_from_this<SendBuffer>
{
public:
	SendBuffer(SendBufferChunkRef owner, BYTE* buffer, int32 allocSize);
	~SendBuffer();


	BYTE* Buffer() { return _buffer; }
	int32 WriteSize() { return _writeSize; }
	void Close(uint32 writeSize);

private:
	BYTE*						_buffer;
	int32						_allocSize= 0;
	int32						_writeSize = 0;//실제사용 사이즈
	SendBufferChunkRef			_owner;
};

/*--------------
	SendBufferChunk 어마어마하게 큰사이즈를 미리 할당 후 잘라서 사용
----------------*/
class SendBufferChunk : public enable_shared_from_this< SendBufferChunk>
{
	enum
	{
		SEND_BUFFER_CHUNK_SIZE = 6000
	};

public:
	SendBufferChunk();
	~SendBufferChunk();

	void									Reset();
	SendBufferRef							Open(uint32 allocSize);
	void									Close(uint32 writeSize);

	bool									 IsOpen() { return _open; }
	BYTE*									 Buffer() { return &_buffer[_usedSize]; }
	uint32									FreeSize() { return  static_cast<uint32>(_buffer.size()) - _usedSize; }

private:
	Array<BYTE, SEND_BUFFER_CHUNK_SIZE>		_buffer = {};
	bool									_open = false;
	uint32									_usedSize = 0;
};

/*--------------
	SendBufferManager
----------------*/
class SendBufferManager
{
public:
	SendBufferRef			Open(uint32 size);

private:
	SendBufferChunkRef			Pop();
	void					Push(SendBufferChunkRef buffer);

	static void				PushGloabal(SendBufferChunk* buffer);

private:
	USE_LOCK;

	vector<SendBufferChunkRef> _sendBufferChunks;

};
