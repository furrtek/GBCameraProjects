#ifndef OSD_H_
#define OSD_H_

void OSD_write_value(const uint8_t px, const uint8_t py, uint8_t* buffer, const char letter, uint32_t value, const uint32_t digits);
void OSD_write(const uint8_t px, const uint8_t py, uint8_t* buffer, char* txt);
void OSD_write_nocam(uint8_t* buffer);

#endif /* OSD_H_ */
