# SPDX-FileCopyrightText: 2023 Stephen Merrony
# SPDX-License-Identifier: MIT

from adafruit_display_text import label
import adafruit_displayio_ssd1306
import adafruit_midi
import adafruit_mpr121
import analogio
import board
import busio
import digitalio
import displayio
import gc
import terminalio
import time
# import touchio
import usb_midi

from adafruit_midi.control_change import ControlChange
from adafruit_midi.note_on import NoteOn
from adafruit_midi.note_off import NoteOff

from micropython import const

VERSION = "v0.0.5"

DEFAULT_CHANNEL = const(0)
BREATH_CC = const(2)
BREATH_CUTOFF = const(20) # Below this, the instrument is silent
ENTER_MENU  = const(-1)
MENU_SELECT = const(-2)
MENU_DOWN   = const(-3)
MENU_UP     = 66 # Same as note F#

TOUCH_THRESHOLD = const(300)

channel = DEFAULT_CHANNEL
transpose = 0

HEIGHT = 64
WIDTH  = 128
BORDER = 5

MENU_REPEAT_PERIOD = 0.4

txpose_dict = {
    -21: "Baritone Sax (EEb)",
    -14: "Bass Clar/Tenor Sax (BBb)",
    -12: "Double Bass (8vb)",
    -9:  "Alto Saxophone (Eb)",
    -7:  "Basset Horn (F)",
    -3:  "A Clarinet",
    -2:  "Bb Clarinet/Sop Sax",
    0:   "C Clarinet (None)",
    3:   "Eb Clarinet",
    12:  "Piccolo (8va)",
}

displayio.release_displays()
i2c = busio.I2C(board.GP1, board.GP0)
display_bus = displayio.I2CDisplay(i2c, device_address=0x3c)
display = adafruit_displayio_ssd1306.SSD1306(display_bus, width=WIDTH, height=HEIGHT)

# Make the display context
screen = displayio.Group()
display.root_group = screen

def splash(scr):
    clarimidi_label = label.Label(
        terminalio.FONT,
        x=10, y = 12,
        text="ClariMIDI",
        color=0,
        scale=2,
        padding_left=1, padding_top=0,
        background_color=0xffffff
    )
    scr.append(clarimidi_label)
    version_label = label.Label(
        terminalio.FONT,
        x=30, y = 40,
        text=VERSION,
        scale=2,
        padding_left=1, padding_top=0,
    )
    scr.append(version_label)
    time.sleep(2)
    scr.remove(version_label)
    scr.remove(clarimidi_label)

# loop waiting for one of the pre-defined menu control key combinations to be pressed.
# Returns a menu action id
def wait_for_menu_key():
    while True:
        if key0.value:
            return MENU_SELECT
        if mpr121[0].value:
            return MENU_UP
        if mpr121[4].value:
            return MENU_DOWN

def transpose_adjust(scr):
    global transpose
    display.root_group = scr
    line2_label = label.Label(terminalio.FONT, x=0, y=25, text=" ", scale=1)
    while True:
        line1_label = label.Label(terminalio.FONT, x=48, y=7, text="%+d" % (transpose), scale=2)
        scr.append(line1_label)
        scr.append(line2_label)
        if transpose in txpose_dict:
            line2_label.text = txpose_dict[transpose]
        else:
            line2_label.text = ""
        cmd = wait_for_menu_key()
        scr.remove(line2_label)
        scr.remove(line1_label)
        if cmd == MENU_UP:
            transpose += 1
        elif cmd == MENU_DOWN:
            transpose -= 1
        else:
            return
        time.sleep(MENU_REPEAT_PERIOD)


def transpose_menu(scr):
    global transpose
    current_line = 1
    display.root_group = scr
    while True:
        if current_line == 1:
            line1_label = label.Label(terminalio.FONT, x=0, y=7, text=">Adjust", scale=2)
        else:
            line1_label = label.Label(terminalio.FONT, x=0, y=7, text=" Adjust", scale=2)
        scr.append(line1_label)

        if current_line == 2:
            line2_label = label.Label(terminalio.FONT, x=0, y=25, text=">Reset", scale=2)
        else:
            line2_label = label.Label(terminalio.FONT, x=0, y=25, text=" Reset", scale=2)
        scr.append(line2_label)

        if current_line == 3:
            line3_label = label.Label(terminalio.FONT, x=0, y=45, text=">Exit", scale=2)
        else:
            line3_label = label.Label(terminalio.FONT, x=0, y=45, text=" Exit", scale=2)
        scr.append(line3_label)

        line4_label = label.Label(terminalio.FONT, x=80, y=52, text="%+d" % (transpose), scale=2)
        scr.append(line4_label)

        cmd = wait_for_menu_key()
        print("DEBUG: Cmd = ", cmd)
        scr.remove(line4_label)
        scr.remove(line3_label)
        scr.remove(line2_label)
        scr.remove(line1_label)
        if cmd == MENU_UP and current_line > 1:     current_line -= 1
        elif cmd == MENU_DOWN and current_line < 3: current_line += 1
        elif cmd == MENU_SELECT:
            print("DEBUG: Menu select")
            if current_line == 1:
                transpose_adjust(scr)
                time.sleep(MENU_REPEAT_PERIOD)
            elif current_line == 2:
                transpose = 0
                time.sleep(MENU_REPEAT_PERIOD)
            else:
                return

def main_menu(scr):
    current_line = 1
    display.root_group = scr
    while True:
        if current_line == 1:
            line1_label = label.Label(terminalio.FONT, x=0, y=7, text=">Transpose", scale=2)
        else:
            line1_label = label.Label(terminalio.FONT, x=0, y=7, text=" Transpose", scale=2)
        scr.append(line1_label)

        if current_line == 2:
            line2_label = label.Label(terminalio.FONT, x=0, y=25, text=">MIDI", scale=2)
        else:
            line2_label = label.Label(terminalio.FONT, x=0, y=25, text=" MIDI", scale=2)
        scr.append(line2_label)

        if current_line == 3:
            line3_label = label.Label(terminalio.FONT, x=0, y=45, text=">Exit", scale=2)
        else:
            line3_label = label.Label(terminalio.FONT, x=0, y=45, text=" Exit", scale=2)
        scr.append(line3_label)

        cmd = wait_for_menu_key()
        print("DEBUG: Cmd = ", cmd)
        scr.remove(line3_label)
        scr.remove(line2_label)
        scr.remove(line1_label)
        if cmd == MENU_UP and current_line > 1:     current_line -= 1
        elif cmd == MENU_DOWN and current_line < 3: current_line += 1
        elif cmd == MENU_SELECT:
            print("DEBUG: Menu select")
            if current_line == 1:
                transpose_menu(scr)
            elif current_line == 2:
                pass # midi_menu(scr)
            else:
                return
        time.sleep(MENU_REPEAT_PERIOD)


breath = analogio.AnalogIn(board.GP28_A2) # Using ADC2/GP28/Phys34 for breath sensor


i2c1 = busio.I2C(board.GP19, board.GP18)
mpr121 = adafruit_mpr121.MPR121(i2c1)

key0 = digitalio.DigitalInOut(board.GP15); key0.direction = digitalio.Direction.INPUT; key0.pull = digitalio.Pull.DOWN;
# key1 = touchio.TouchIn(board.GP16); key1.threshold += TOUCH_THRESHOLD   # Thumb
key2 = digitalio.DigitalInOut(board.GP14); key2.direction = digitalio.Direction.INPUT; key2.pull = digitalio.Pull.DOWN;
key3 = digitalio.DigitalInOut(board.GP13); key3.direction = digitalio.Direction.INPUT; key3.pull = digitalio.Pull.DOWN;
# key4 = touchio.TouchIn(board.GP13); key4.threshold += TOUCH_THRESHOLD   # L1
# key5 = touchio.TouchIn(board.GP18); key5.threshold += TOUCH_THRESHOLD   # L2
key6 = digitalio.DigitalInOut(board.GP12); key6.direction = digitalio.Direction.INPUT; key6.pull = digitalio.Pull.DOWN;
# key7 = touchio.TouchIn(board.GP19); key7.threshold += TOUCH_THRESHOLD   # L3
key8 = digitalio.DigitalInOut(board.GP6);  key8.direction = digitalio.Direction.INPUT; key8.pull = digitalio.Pull.DOWN;
key9 = digitalio.DigitalInOut(board.GP21); key9.direction = digitalio.Direction.INPUT; key9.pull = digitalio.Pull.DOWN;
key10 = digitalio.DigitalInOut(board.GP10); key10.direction = digitalio.Direction.INPUT; key10.pull = digitalio.Pull.DOWN;
key11 = digitalio.DigitalInOut(board.GP20); key11.direction = digitalio.Direction.INPUT; key11.pull = digitalio.Pull.DOWN;
# key12 = touchio.TouchIn(board.GP9); key12.threshold += TOUCH_THRESHOLD  # R1
# key13 = touchio.TouchIn(board.GP22); key13.threshold += TOUCH_THRESHOLD # R2
key14 = digitalio.DigitalInOut(board.GP8); key14.direction = digitalio.Direction.INPUT; key14.pull = digitalio.Pull.DOWN;
# key15 = touchio.TouchIn(board.GP7); key15.threshold += TOUCH_THRESHOLD  # R3
key16 = digitalio.DigitalInOut(board.GP7); key16.direction = digitalio.Direction.INPUT; key16.pull = digitalio.Pull.DOWN;
key17 = digitalio.DigitalInOut(board.GP5); key17.direction = digitalio.Direction.INPUT; key17.pull = digitalio.Pull.DOWN;
key18 = digitalio.DigitalInOut(board.GP4); key18.direction = digitalio.Direction.INPUT; key18.pull = digitalio.Pull.DOWN;
key19 = digitalio.DigitalInOut(board.GP3); key19.direction = digitalio.Direction.INPUT; key19.pull = digitalio.Pull.DOWN;
key20 = digitalio.DigitalInOut(board.GP2); key20.direction = digitalio.Direction.INPUT; key20.pull = digitalio.Pull.DOWN;

splash(screen)
gc.collect()

midi = adafruit_midi.MIDI(midi_out=usb_midi.ports[1], out_channel=channel, debug=False)

# N.B. These note values are effectively for 'clarinet in C'
notedict = {
    #   098765432109876543210
    #   EEFFG3 21FBC 3 21 ATR
    #   b  ##    #b      b
    0b00000000000000000000000: (67,  "g4"),
    0b00000000000000000000010: (65,  "f4"),
    0b00000000000000000000011: (84,  "c6"),
    0b00000000000000000000100: (69,  "a4"),
    0b00000000000000000000101: (70,  "bb4"), # normal
    0b00000000000000000001000: (68,  "g#4"),
    0b00000000000000000001100: (69,  "a4"),  # normal plus A-flat key"
    0b00000000000000000010000: (66,  "f#4"), # also, menu-up
    0b00000000000000000010010: (64,  "e4"),
    0b00000000000000000010011: (83,  "b5"),
    0b00000000000000000100011: (84,  "C6"),
    0b00000000000000000110010: (62,  "d4"),
    0b00000000000000000110011: (81,  "a5"),
    0b00000000000000001110010: (63,  "d#4"), # Front & SK1
    0b00000000000000001110011: (82,  "a#5"), # Front & SK1
    0b00000000000000010110010: (60,  "c4"),  # Middle C
    0b00000000000000010110011: (79,  "g5"),
    0b00000000000000110110010: (61,  "c#4"),
    0b00000000000000110110011: (80,  "g#5"),
    0b00000000000001000000100: (72,  "c5"),  # SK4
    0b00000000000010000000100: (70,  "bb4"), # SK3
    0b00000000000100000000010: (66,  "f#4"), # SK2
    0b00000000001000010110010: (58,  "bb3"),
    0b00000000001000010110011: (77,  "f5"),
    0b00000000010000010110010: (59,  "b3"),
    0b00000000010000010110011: (78,  "f#5"),
    0b00000000011000010110010: (57,  "a3"),
    0b00000000011000010110011: (76,  "e5"),
    0b00000000011000010100011: (85,  "c#6"),
    0b00000001011000010110010: (55,  "g3"),
    0b00000001011000010110011: (74,  "d5"),
    0b00000011011000010110010: (56,  "g#3"),
    0b00000011011000010110011: (75,  "d#5"),
    0b00001001011000010110010: (53,  "f3"),
    0b00001001011000010110011: (72,  "c5"),
    0b00010001011000010110010: (52,  "e3"),
    0b00010001011000010110011: (71,  "b5"),
    0b00100001011000010110010: (51,  "eb3"),
    #
    #   098765432109876543210
    #   EEFFG3 21FBC 3 21 ATR
    #   b  ##    #b      b
    0b00010010000000000000000: (ENTER_MENU, "Menu"), # Enter menu command
    0b00000000000000000000001: (MENU_SELECT, "Select"),
    0b00000000000000000100000: (MENU_DOWN, "Down"),
}

last_breath = 0
last_keys = 0
last_midi_note = 60
sum = 0
last_sum = 0
curr_breath = 0
midi_note = notedict[sum][0]

while True:
    curr_breath = 127 - (breath.value // 512)
    if curr_breath != last_breath:
        if curr_breath < BREATH_CUTOFF:     # stop the sound if below threshold
            midi.send(NoteOff(last_midi_note + transpose))
            curr_breath = 0
            # print("DEBUG: Breath pressure changed to:", curr_breath)
        else:
            if last_breath == 0:            # restart the sound if risen above threshold
                midi.send(NoteOn(last_midi_note + transpose, curr_breath))
            else:
                midi.send(ControlChange(BREATH_CC, curr_breath))
            print("DEBUG: Breath pressure changed to:", curr_breath)
        last_breath = curr_breath
    # print("DEBUG: Breath pressure changed to:", curr_breath)
    touched = mpr121.touched_pins
    sum = 0
    if key0.value:
        sum = 0b1
    if touched[2]:
        sum += 0b10
    if key2.value:
        sum += 0b100
    if key3.value:
        sum += 0b1000
    if touched[0]:
        sum += 0b10000
    if touched[4]:
        sum += 0b100000
    if key6.value:
        sum += 0b1000000
    if touched[6]:
        sum += 0b10000000
    if key8.value:
        sum += 0b100000000
    if key9.value:
        sum += 0b1000000000
    if key10.value:
        sum += 0b10000000000
    if key11.value:
        sum += 0b100000000000
    if touched[8]:
        sum += 0b1000000000000
    if touched[10]:
        sum += 0b10000000000000
    if key14.value:
        sum += 0b100000000000000
    if touched[11]:
        sum += 0b1000000000000000
    if key16.value:
        sum += 0b10000000000000000
    if key17.value:
        sum += 0b100000000000000000
    if key18.value:
        sum += 0b1000000000000000000    
    if key19.value:
        sum += 0b10000000000000000000
    if key20.value:
        sum += 0b100000000000000000000
    if sum != last_keys:
        if sum in notedict:
            midi_note = notedict[sum][0]
            if midi_note == ENTER_MENU:
                main_menu(screen)
                gc.collect()
            elif midi_note > 0:
                midi.send(NoteOff(last_midi_note + transpose))
                midi.send(NoteOn(midi_note + transpose, last_breath))
                print("DEBUG: Keys changed to:", bin(sum),
                    " giving MIDI note:", midi_note,
                    " i.e.:", notedict[sum][1])
                last_keys = sum
                last_midi_note = midi_note
        elif sum != last_sum:
            print("DEBUG: Ignoring unknown key combo:", bin(sum))
            print("DEBUG: Free memory: {} bytes".format(gc.mem_free()))
            last_sum = sum

    time.sleep(0.01)
