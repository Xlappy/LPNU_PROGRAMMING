# Makefile for Arduino Uno Projects (PlatformIO)

# Variables
PIO = pio

# Actions
.PHONY: build_dht status build_rfid upload_dht upload_rfid monitor clean

help:
	@echo "Commands:"
	@echo "  make build_dht   - Build Lab 1 (DHT11)"
	@echo "  make build_rfid  - Build Lab 2 (RFID)"
	@echo "  make upload_dht  - Upload Lab 1 to Uno"
	@echo "  make upload_rfid - Upload Lab 2 to Uno"
	@echo "  make monitor     - Open Serial Monitor"

build_dht:
	@echo "Building Lab 1 (DHT11)..."
	# Temporarily rename or move files if necessary if you want single source builds
	# But PIO usually builds everything in src. For labs, we might need to handle this.
	$(PIO) run -e uno

build_rfid:
	@echo "Building Lab 2 (RFID)..."
	$(PIO) run -e uno

upload_dht:
	@echo "Uploading Lab 1..."
	$(PIO) run -e uno --target upload

upload_rfid:
	@echo "Uploading Lab 2..."
	$(PIO) run -e uno --target upload

monitor:
	$(PIO) device monitor

clean:
	$(PIO) run --target clean
