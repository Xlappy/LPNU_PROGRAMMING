# Makefile for Arduino Uno Projects (PlatformIO)

# Variables
# Variables
PIO := $(shell which pio 2>/dev/null || ls /Users/yurakozichuk/Library/Python/3.9/bin/pio 2>/dev/null || echo "$$HOME/.platformio/penv/bin/pio")

# Actions
.PHONY: build_dht status build_rfid upload_dht upload_rfid monitor clean check_pio erase

check_pio:
	@if [ ! -x "$(PIO)" ] && ! command -v pio >/dev/null 2>&1; then \
		echo "Error: 'pio' command not found."; \
		echo "Please use the PlatformIO icons (Checkmark/Arrow) in the VS Code bottom bar."; \
		echo "Or open the 'PlatformIO Core CLI' terminal in VS Code and run 'make' there."; \
		exit 1; \
	fi

help:
	@echo "Commands:"
	@echo "  make build_dht   - Build Lab 1 (DHT11)"
	@echo "  make build_rfid  - Build Lab 2 (RFID)"
	@echo "  make upload_dht  - Upload Lab 1 to Uno"
	@echo "  make upload_rfid - Upload Lab 2 to Uno"
	@echo "  make monitor     - Open Serial Monitor"

build_dht: check_pio
	@echo "Building Lab 1 (DHT11)..."
	@mv src/lab2_rfid.cpp src/lab2_rfid.cpp.tmp 2>/dev/null || true
	@$(PIO) run -e uno
	@mv src/lab2_rfid.cpp.tmp src/lab2_rfid.cpp 2>/dev/null || true

build_rfid: check_pio
	@echo "Building Lab 2 (RFID)..."
	@mv src/lab1_dht11.cpp src/lab1_dht11.cpp.tmp 2>/dev/null || true
	@$(PIO) run -e uno
	@mv src/lab1_dht11.cpp.tmp src/lab1_dht11.cpp 2>/dev/null || true

upload_dht: check_pio
	@echo "Uploading Lab 1..."
	@mv src/lab2_rfid.cpp src/lab2_rfid.cpp.tmp 2>/dev/null || true
	@$(PIO) run -e uno --target upload
	@mv src/lab2_rfid.cpp.tmp src/lab2_rfid.cpp 2>/dev/null || true

upload_rfid: check_pio
	@echo "Uploading Lab 2..."
	@mv src/lab1_dht11.cpp src/lab1_dht11.cpp.tmp 2>/dev/null || true
	@$(PIO) run -e uno --target upload
	@mv src/lab1_dht11.cpp.tmp src/lab1_dht11.cpp 2>/dev/null || true

monitor:
	$(PIO) device monitor

clean:
	$(PIO) run --target clean

erase: check_pio
	@echo "Erasing Arduino (uploading empty sketch)..."
	@mkdir -p src/tmp_erase
	@echo "void setup(){} void loop(){}" > src/tmp_erase/empty.cpp
	@mv src/*.cpp src/tmp_erase/ 2>/dev/null || true
	@mv src/tmp_erase/empty.cpp src/
	@$(PIO) run -e uno --target upload
	@rm src/empty.cpp
	@mv src/tmp_erase/*.cpp src/ 2>/dev/null || true
	@rmdir src/tmp_erase
