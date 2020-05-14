#include <iostream>
#include <string>
#include <vector>

#include "./messages.hpp"
#include "./serializers.hpp"

extern int test_libcore(int x) { return x * 123; };

MessageBuilder::MessageBuilder() {
  buffer_ = std::vector<u_int8_t>(DATA_OFFSET_);
  data_len_ = 0;
};

void MessageBuilder::writeMessageType(MessageType type) {
  buffer_[MESSAGE_TYPE_OFFSET_] = (u_int8_t)type;
};

void MessageBuilder::write(u_int8_t x) {
  buffer_.push_back(x);
  data_len_ += 1;
};

void MessageBuilder::write(off_t x) {
  for (int i = 0; i < 4; i++)
    buffer_.push_back(x >> ((3 - i) * 8));
  data_len_ += 4;
};


void MessageBuilder::write(int32_t x) {
  for (int i = 0; i < 4; i++)
    buffer_.push_back(x >> ((3 - i) * 8));
  data_len_ += 4;
};

void MessageBuilder::write(u_int32_t x) {
  for (int i = 0; i < 4; i++)
    buffer_.push_back(x >> ((3 - i) * 8));
  data_len_ += 4;
};

void MessageBuilder::write(u_int64_t x) {
  for (int i = 0; i < 8; i++)
    buffer_.push_back(x >> ((7 - i) * 8));
  data_len_ += 8;
};

void MessageBuilder::write(char *buf, size_t size) {
  write((u_int32_t)size);

  for (int i = 0; i < size; i++)
    buffer_.push_back(buf[i]);
  data_len_ += size;
};

void MessageBuilder::write(std::vector<u_int8_t> bytes) {
  write((u_int32_t)bytes.size());

  for (int i = 0; i < bytes.size(); i++)
    buffer_.push_back(bytes[i]);
  data_len_ += bytes.size();
};

void MessageBuilder::write(std::string str) {
  write((u_int32_t)str.size());

  for (char c : str)
    buffer_.push_back(c);
  data_len_ += str.size();
};
/*
void MessageBuilder::write(off_t fs) {
  for (int i = 0; i < 4; i++)
    buffer_.push_back(fs >> ((3 - i) * 8));
  data_len_ += 4;
};
*/
std::vector<u_int8_t> MessageBuilder::build() {
  for (int i = 0; i < 4; i++)
    buffer_[DATA_SIZE_OFFSET_ + i] = (data_len_ >> ((3 - i) * 8));

  return buffer_;
};

MessageParser::MessageParser(std::vector<u_int8_t> byte_message) {
  buffer_ = byte_message;
  next_data_ = DATA_OFFSET_;
};

MessageType MessageParser::readMessageType() {
  MessageType result = static_cast<MessageType>(buffer_[MESSAGE_TYPE_OFFSET_]);
  return result;
};

int32_t MessageParser::readSize() {
  int32_t result = 0;
  for (int i = 0; i < 4; i++)
    result +=
        (static_cast<int32_t>(buffer_[DATA_SIZE_OFFSET_ + i]) << ((3 - i) * 8));

  return result;
};

u_int8_t MessageParser::readUInt8T() {
  u_int8_t result = buffer_[next_data_];
  next_data_ += 1;

  return result;
};

int32_t MessageParser::readInt32T() {
  int32_t result = 0;
  for (int i = 0; i < 4; i++, next_data_++)
    result += (static_cast<int32_t>(buffer_[next_data_]) << ((3 - i) * 8));

  return result;
};

off_t MessageParser::readOffT() {
  off_t result = 0;
  for (int i = 0; i < 4; i++, next_data_++)
    result += (static_cast<off_t>(buffer_[next_data_]) << ((3 - i) * 8));

  return result;
};

u_int32_t MessageParser::readUInt32T() {
  int32_t result = 0;
  for (int i = 0; i < 4; i++, next_data_++)
    result += (static_cast<u_int32_t>(buffer_[next_data_]) << ((3 - i) * 8));

  return result;
};

u_int64_t MessageParser::readUInt64T() {
  u_int64_t result = 0;
  for (int i = 0; i < 8; i++, next_data_++)
    result += (static_cast<u_int64_t>(buffer_[next_data_]) << ((7 - i) * 8));

  return result;
};

std::vector<u_int8_t> MessageParser::readBytes() {
  u_int32_t result_size = readUInt32T();

  std::vector<u_int8_t> result;
  for (int i = 0; i < result_size; i++, next_data_++)
    result.push_back(buffer_[next_data_]);

  return result;
};

std::string MessageParser::readString() {
  u_int32_t result_size = readUInt32T();

  std::string result;
  for (int i = 0; i < result_size; i++, next_data_++)
    result.push_back(buffer_[next_data_]);

  return result;
};

extern std::vector<u_int8_t> SerializeOpenRequest(OpenRequest open_request) {
  MessageType message_type = MessageType::OPEN_REQUEST;
  std::string path = open_request.path;
  int32_t oflag = open_request.oflag;
  int32_t mode = open_request.mode;

  MessageBuilder request_builder;
  request_builder.writeMessageType(message_type);
  request_builder.write(path);
  request_builder.write(oflag);
  request_builder.write(mode);

  std::vector<u_int8_t> byte_request = request_builder.build();
  return byte_request;
};
extern OpenRequest
DeserializeToOpenRequest(std::vector<u_int8_t> byte_request) {
  MessageParser request_parser(byte_request);
  MessageType message_type = request_parser.readMessageType();
  std::string path = request_parser.readString();
  int32_t oflag = request_parser.readInt32T();
  int32_t mode = request_parser.readInt32T();

  OpenRequest open_request{path, oflag, mode};
  return open_request;
};

extern std::vector<u_int8_t> SerializeOpenResponse(OpenResponse open_response) {
  MessageType message_type = MessageType::OPEN_RESPONSE;
  int32_t result = open_response.result;
  int32_t error = open_response.error;

  MessageBuilder request_builder;
  request_builder.writeMessageType(message_type);
  request_builder.write(result);
  request_builder.write(error);

  std::vector<u_int8_t> byte_request = request_builder.build();
  return byte_request;
};

extern OpenResponse
DeserializeToOpenResponse(std::vector<u_int8_t> byte_response) {
  MessageParser request_parser(byte_response);
  MessageType message_type = request_parser.readMessageType();
  int32_t result = request_parser.readInt32T();
  int32_t error = request_parser.readInt32T();

  OpenResponse open_response{result, error};
  return open_response;
};

extern std::vector<u_int8_t> SerializeReadRequest(ReadRequest read_request) {
  MessageType message_type = MessageType::READ_REQUEST;
  int32_t fd = read_request.fd;
  int32_t count = read_request.count;

  MessageBuilder request_builder;
  request_builder.writeMessageType(message_type);
  request_builder.write(fd);
  request_builder.write(count);

  std::vector<u_int8_t> byte_request = request_builder.build();
  return byte_request;
};

extern ReadRequest DeserializeToReadRequest(std::vector<u_int8_t> byte_request) {
  MessageParser request_parser(byte_request);
  MessageType message_type = request_parser.readMessageType();
  int32_t fd = request_parser.readInt32T();
  int32_t count = request_parser.readInt32T();

  ReadRequest read_request{fd, count};
  return read_request;
};

extern std::vector<u_int8_t> SerializeReadResponse(ReadResponse read_response) {
  MessageType message_type = MessageType::READ_RESPONSE;
  int32_t result = read_response.result;
  std::vector<u_int8_t> buf = read_response.buf;
  int32_t error = read_response.error;

  MessageBuilder response_builder;
  response_builder.writeMessageType(message_type);
  response_builder.write(result);
  response_builder.write(buf);
  response_builder.write(error);

  std::vector<u_int8_t> byte_response = response_builder.build();
  return byte_response;
};

extern ReadResponse DeserializeToReadResponse(std::vector<u_int8_t> byte_response) {
  MessageParser response_parser(byte_response);
  MessageType message_type = response_parser.readMessageType();
  int32_t result = response_parser.readInt32T();
  std::vector<u_int8_t> buf  = response_parser.readBytes();
  int32_t error = response_parser.readInt32T();

  ReadResponse read_response{result, buf, error};
  return read_response;
};

//+
extern std::vector<u_int8_t> SerializeWriteRequest(WriteRequest write_request) {
  MessageType message_type = MessageType::WRITE_REQUEST;
  int32_t fd = write_request.fd;
  std::vector<u_int8_t> buf = write_request.buf;

  MessageBuilder request_builder;
  request_builder.writeMessageType(message_type);
  request_builder.write(fd);
  request_builder.write(buf);

  std::vector<u_int8_t> byte_request = request_builder.build();
  return byte_request;
};
//+
extern WriteRequest DeserializeToWriteRequest(std::vector<u_int8_t> byte_request) {
  MessageParser request_parser(byte_request);
  MessageType message_type = request_parser.readMessageType();
  int32_t fd = request_parser.readInt32T();
  std::vector<u_int8_t> buf = request_parser.readBytes();

  WriteRequest write_request{fd, buf};
  return write_request;
};
//+
extern std::vector<u_int8_t> SerializeWriteResponse(WriteResponse write_response) {
  MessageType message_type = MessageType::WRITE_RESPONSE;
  int32_t result = write_response.result;
  int32_t error = write_response.error;

  MessageBuilder response_builder;
  response_builder.writeMessageType(message_type);
  response_builder.write(result);
  response_builder.write(error);

  std::vector<u_int8_t> byte_response = response_builder.build();
  return byte_response;
};

//+
extern WriteResponse DeserializeToWriteResponse(std::vector<u_int8_t> byte_response) {
  MessageParser response_parser(byte_response);
  MessageType message_type = response_parser.readMessageType();
  int32_t result = response_parser.readInt32T();
  int32_t error = response_parser.readInt32T();

  WriteResponse write_response{result, error};
  return write_response;
};

//+
extern std::vector<u_int8_t> SerializeLseekRequest(LseekRequest lseek_request) {
  MessageType message_type = MessageType::LSEEK_REQUEST;
  int32_t fd = lseek_request.fd;
  off_t offset = lseek_request.offset;
  int32_t whence = lseek_request.whence;

  MessageBuilder request_builder;
  request_builder.writeMessageType(message_type);
  request_builder.write(fd);
  request_builder.write(offset);
  request_builder.write(whence);

  std::vector<u_int8_t> byte_request = request_builder.build();
  return byte_request;
};
//+
extern LseekRequest DeserializeToLseekRequest(std::vector<u_int8_t> byte_request) {
  MessageParser request_parser(byte_request);
  MessageType message_type = request_parser.readMessageType();
  int32_t fd = request_parser.readInt32T();
  off_t offset = request_parser.readOffT();
  int32_t whence = request_parser.readInt32T();

  LseekRequest lseek_request{fd, offset, whence};
  return lseek_request;
};
//+
extern std::vector<u_int8_t> SerializeLseekResponse(LseekResponse lseek_response) {
  MessageType message_type = MessageType::LSEEK_RESPONSE;
  off_t result = lseek_response.result; //?
  int32_t error = lseek_response.error;

  MessageBuilder response_builder;
  response_builder.writeMessageType(message_type);
  response_builder.write(result);
  response_builder.write(error);

  std::vector<u_int8_t> byte_response = response_builder.build();
  return byte_response;
};
//+
extern LseekResponse DeserializeToLseekResponse(std::vector<u_int8_t> byte_response) {
  MessageParser response_parser(byte_response);
  MessageType message_type = response_parser.readMessageType();
  off_t result = response_parser.readOffT(); //??
  int32_t error = response_parser.readInt32T();

  LseekResponse lseek_response{result, error};
  return lseek_response;
};

//
extern std::vector<u_int8_t> SerializeCloseRequest(CloseRequest close_request) {
  MessageType message_type = MessageType::CLOSE_REQUEST;
  int32_t fd = close_request.fd;

  MessageBuilder request_builder;
  request_builder.writeMessageType(message_type);
  request_builder.write(fd);

  std::vector<u_int8_t> byte_request = request_builder.build();
  return byte_request;
};
//
extern CloseRequest DeserializeToCloseRequest(std::vector<u_int8_t> byte_request) {
  MessageParser request_parser(byte_request);
  MessageType message_type = request_parser.readMessageType();
  int32_t fd = request_parser.readInt32T();

  CloseRequest close_request{fd};
  return close_request;
};
//
extern std::vector<u_int8_t> SerializeCloseResponse(CloseResponse close_response) {
  MessageType message_type = MessageType::CLOSE_RESPONSE;
  int32_t result = close_response.result;
  int32_t error = close_response.error;

  MessageBuilder response_builder;
  response_builder.writeMessageType(message_type);
  response_builder.write(result);
  response_builder.write(error);

  std::vector<u_int8_t> byte_response = response_builder.build();
  return byte_response;
};
//
extern CloseResponse DeserializeToCloseResponse(std::vector<u_int8_t> byte_response) {
  MessageParser response_parser(byte_response);
  MessageType message_type = response_parser.readMessageType();
  int32_t result = response_parser.readInt32T();
  int32_t error = response_parser.readInt32T();

  CloseResponse close_response{result, error};
  return close_response;
};