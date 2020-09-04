# our C++ callback
.extern isrcallback

.global isr0
.global isr1
.global isrwrapper_return

.section .text
.align 4

# Note: The first 18 ISR calls are CPU exceptions !
isr0:
pushl $0
jmp isrwrapper
isr1:
pushl $1
jmp isrwrapper
isr2:

pushl $2
jmp isrwrapper
isr3:

pushl $3
jmp isrwrapper
isr4:

pushl $4
jmp isrwrapper
isr5:

pushl $5
jmp isrwrapper
isr6:

pushl $6
jmp isrwrapper
isr7:

pushl $7
jmp isrwrapper
isr8:
# This exception pushes an error code by it's self!
pushl $8
jmp isrwrapper
isr9:

pushl $9
jmp isrwrapper
isr10:
# This exception pushes an error code by it's self!
pushl $10
jmp isrwrapper
isr11:
# This exception pushes an error code by it's self!
pushl $11
jmp isrwrapper
isr12:
# This exception pushes an error code by it's self!
pushl $12
jmp isrwrapper
isr13:
# This exception pushes an error code by it's self!
pushl $13
jmp isrwrapper
isr14:
# This exception pushes an error code by it's self!
pushl $14
jmp isrwrapper
isr15:

pushl $15
jmp isrwrapper
isr16:

pushl $16
jmp isrwrapper
isr17:
# This exception pushes an error code by it's self!
pushl $17
jmp isrwrapper
isr18:

pushl $18
jmp isrwrapper
isr19:

pushl $19
jmp isrwrapper
isr20:

pushl $20
jmp isrwrapper
isr21:

pushl $21
jmp isrwrapper
isr22:

pushl $22
jmp isrwrapper
isr23:

pushl $23
jmp isrwrapper
isr24:

pushl $24
jmp isrwrapper
isr25:

pushl $25
jmp isrwrapper
isr26:

pushl $26
jmp isrwrapper
isr27:

pushl $27
jmp isrwrapper
isr28:

pushl $28
jmp isrwrapper
isr29:

pushl $29
jmp isrwrapper
isr30:

pushl $30
jmp isrwrapper
isr31:

pushl $31
jmp isrwrapper

# Start of Hardware Interrupts aka IRQ
isr32:

pushl $32
jmp isrwrapper
isr33:

pushl $33
jmp isrwrapper
isr34:

pushl $34
jmp isrwrapper
isr35:

pushl $35
jmp isrwrapper
isr36:

pushl $36
jmp isrwrapper
isr37:

pushl $37
jmp isrwrapper
isr38:

pushl $38
jmp isrwrapper
isr39:

pushl $39
jmp isrwrapper
isr40:

pushl $40
jmp isrwrapper
isr41:

pushl $41
jmp isrwrapper
isr42:

pushl $42
jmp isrwrapper
isr43:

pushl $43
jmp isrwrapper
isr44:

pushl $44
jmp isrwrapper
isr45:

pushl $45
jmp isrwrapper
isr46:

pushl $46
jmp isrwrapper
isr47:

pushl $47
jmp isrwrapper

# Free Interrupts used for system calls and so on
isr48:

pushl $48
jmp isrwrapper
isr49:

pushl $49
jmp isrwrapper
isr50:

pushl $50
jmp isrwrapper
isr51:

pushl $51
jmp isrwrapper
isr52:

pushl $52
jmp isrwrapper
isr53:

pushl $53
jmp isrwrapper
isr54:

pushl $54
jmp isrwrapper
isr55:

pushl $55
jmp isrwrapper
isr56:

pushl $56
jmp isrwrapper
isr57:

pushl $57
jmp isrwrapper
isr58:

pushl $58
jmp isrwrapper
isr59:

pushl $59
jmp isrwrapper
isr60:

pushl $60
jmp isrwrapper
isr61:

pushl $61
jmp isrwrapper
isr62:

pushl $62
jmp isrwrapper
isr63:

pushl $63
jmp isrwrapper
isr64:

pushl $64
jmp isrwrapper
isr65:

pushl $65
jmp isrwrapper
isr66:

pushl $66
jmp isrwrapper
isr67:

pushl $67
jmp isrwrapper
isr68:

pushl $68
jmp isrwrapper
isr69:

pushl $69
jmp isrwrapper
isr70:

pushl $70
jmp isrwrapper
isr71:

pushl $71
jmp isrwrapper
isr72:

pushl $72
jmp isrwrapper
isr73:

pushl $73
jmp isrwrapper
isr74:

pushl $74
jmp isrwrapper
isr75:

pushl $75
jmp isrwrapper
isr76:

pushl $76
jmp isrwrapper
isr77:

pushl $77
jmp isrwrapper
isr78:

pushl $78
jmp isrwrapper
isr79:

pushl $79
jmp isrwrapper
isr80:

pushl $80
jmp isrwrapper
isr81:

pushl $81
jmp isrwrapper
isr82:

pushl $82
jmp isrwrapper
isr83:

pushl $83
jmp isrwrapper
isr84:

pushl $84
jmp isrwrapper
isr85:

pushl $85
jmp isrwrapper
isr86:

pushl $86
jmp isrwrapper
isr87:

pushl $87
jmp isrwrapper
isr88:

pushl $88
jmp isrwrapper
isr89:

pushl $89
jmp isrwrapper
isr90:

pushl $90
jmp isrwrapper
isr91:

pushl $91
jmp isrwrapper
isr92:

pushl $92
jmp isrwrapper
isr93:

pushl $93
jmp isrwrapper
isr94:

pushl $94
jmp isrwrapper
isr95:

pushl $95
jmp isrwrapper
isr96:

pushl $96
jmp isrwrapper
isr97:

pushl $97
jmp isrwrapper
isr98:

pushl $98
jmp isrwrapper
isr99:

pushl $99
jmp isrwrapper
isr100:

pushl $100
jmp isrwrapper
isr101:

pushl $101
jmp isrwrapper
isr102:

pushl $102
jmp isrwrapper
isr103:

pushl $103
jmp isrwrapper
isr104:

pushl $104
jmp isrwrapper
isr105:

pushl $105
jmp isrwrapper
isr106:

pushl $106
jmp isrwrapper
isr107:

pushl $107
jmp isrwrapper
isr108:

pushl $108
jmp isrwrapper
isr109:

pushl $109
jmp isrwrapper
isr110:

pushl $110
jmp isrwrapper
isr111:

pushl $111
jmp isrwrapper
isr112:

pushl $112
jmp isrwrapper
isr113:

pushl $113
jmp isrwrapper
isr114:

pushl $114
jmp isrwrapper
isr115:

pushl $115
jmp isrwrapper
isr116:

pushl $116
jmp isrwrapper
isr117:

pushl $117
jmp isrwrapper
isr118:

pushl $118
jmp isrwrapper
isr119:

pushl $119
jmp isrwrapper
isr120:

pushl $120
jmp isrwrapper
isr121:

pushl $121
jmp isrwrapper
isr122:

pushl $122
jmp isrwrapper
isr123:

pushl $123
jmp isrwrapper
isr124:

pushl $124
jmp isrwrapper
isr125:

pushl $125
jmp isrwrapper
isr126:

pushl $126
jmp isrwrapper
isr127:

pushl $127
jmp isrwrapper
isr128:

pushl $128
jmp isrwrapper
isr129:

pushl $129
jmp isrwrapper
isr130:

pushl $130
jmp isrwrapper
isr131:

pushl $131
jmp isrwrapper
isr132:

pushl $132
jmp isrwrapper
isr133:

pushl $133
jmp isrwrapper
isr134:

pushl $134
jmp isrwrapper
isr135:

pushl $135
jmp isrwrapper
isr136:

pushl $136
jmp isrwrapper
isr137:

pushl $137
jmp isrwrapper
isr138:

pushl $138
jmp isrwrapper
isr139:

pushl $139
jmp isrwrapper
isr140:

pushl $140
jmp isrwrapper
isr141:

pushl $141
jmp isrwrapper
isr142:

pushl $142
jmp isrwrapper
isr143:

pushl $143
jmp isrwrapper
isr144:

pushl $144
jmp isrwrapper
isr145:

pushl $145
jmp isrwrapper
isr146:

pushl $146
jmp isrwrapper
isr147:

pushl $147
jmp isrwrapper
isr148:

pushl $148
jmp isrwrapper
isr149:

pushl $149
jmp isrwrapper
isr150:

pushl $150
jmp isrwrapper
isr151:

pushl $151
jmp isrwrapper
isr152:

pushl $152
jmp isrwrapper
isr153:

pushl $153
jmp isrwrapper
isr154:

pushl $154
jmp isrwrapper
isr155:

pushl $155
jmp isrwrapper
isr156:

pushl $156
jmp isrwrapper
isr157:

pushl $157
jmp isrwrapper
isr158:

pushl $158
jmp isrwrapper
isr159:

pushl $159
jmp isrwrapper
isr160:

pushl $160
jmp isrwrapper
isr161:

pushl $161
jmp isrwrapper
isr162:

pushl $162
jmp isrwrapper
isr163:

pushl $163
jmp isrwrapper
isr164:

pushl $164
jmp isrwrapper
isr165:

pushl $165
jmp isrwrapper
isr166:

pushl $166
jmp isrwrapper
isr167:

pushl $167
jmp isrwrapper
isr168:

pushl $168
jmp isrwrapper
isr169:

pushl $169
jmp isrwrapper
isr170:

pushl $170
jmp isrwrapper
isr171:

pushl $171
jmp isrwrapper
isr172:

pushl $172
jmp isrwrapper
isr173:

pushl $173
jmp isrwrapper
isr174:

pushl $174
jmp isrwrapper
isr175:

pushl $175
jmp isrwrapper
isr176:

pushl $176
jmp isrwrapper
isr177:

pushl $177
jmp isrwrapper
isr178:

pushl $178
jmp isrwrapper
isr179:

pushl $179
jmp isrwrapper
isr180:

pushl $180
jmp isrwrapper
isr181:

pushl $181
jmp isrwrapper
isr182:

pushl $182
jmp isrwrapper
isr183:

pushl $183
jmp isrwrapper
isr184:

pushl $184
jmp isrwrapper
isr185:

pushl $185
jmp isrwrapper
isr186:

pushl $186
jmp isrwrapper
isr187:

pushl $187
jmp isrwrapper
isr188:

pushl $188
jmp isrwrapper
isr189:

pushl $189
jmp isrwrapper
isr190:

pushl $190
jmp isrwrapper
isr191:

pushl $191
jmp isrwrapper
isr192:

pushl $192
jmp isrwrapper
isr193:

pushl $193
jmp isrwrapper
isr194:

pushl $194
jmp isrwrapper
isr195:

pushl $195
jmp isrwrapper
isr196:

pushl $196
jmp isrwrapper
isr197:

pushl $197
jmp isrwrapper
isr198:

pushl $198
jmp isrwrapper
isr199:

pushl $199
jmp isrwrapper
isr200:

pushl $200
jmp isrwrapper
isr201:

pushl $201
jmp isrwrapper
isr202:

pushl $202
jmp isrwrapper
isr203:

pushl $203
jmp isrwrapper
isr204:

pushl $204
jmp isrwrapper
isr205:

pushl $205
jmp isrwrapper
isr206:

pushl $206
jmp isrwrapper
isr207:

pushl $207
jmp isrwrapper
isr208:

pushl $208
jmp isrwrapper
isr209:

pushl $209
jmp isrwrapper
isr210:

pushl $210
jmp isrwrapper
isr211:

pushl $211
jmp isrwrapper
isr212:

pushl $212
jmp isrwrapper
isr213:

pushl $213
jmp isrwrapper
isr214:

pushl $214
jmp isrwrapper
isr215:

pushl $215
jmp isrwrapper
isr216:

pushl $216
jmp isrwrapper
isr217:

pushl $217
jmp isrwrapper
isr218:

pushl $218
jmp isrwrapper
isr219:

pushl $219
jmp isrwrapper
isr220:

pushl $220
jmp isrwrapper
isr221:

pushl $221
jmp isrwrapper
isr222:

pushl $222
jmp isrwrapper
isr223:

pushl $223
jmp isrwrapper
isr224:

pushl $224
jmp isrwrapper
isr225:

pushl $225
jmp isrwrapper
isr226:

pushl $226
jmp isrwrapper
isr227:

pushl $227
jmp isrwrapper
isr228:

pushl $228
jmp isrwrapper
isr229:

pushl $229
jmp isrwrapper
isr230:

pushl $230
jmp isrwrapper
isr231:

pushl $231
jmp isrwrapper
isr232:

pushl $232
jmp isrwrapper
isr233:

pushl $233
jmp isrwrapper
isr234:

pushl $234
jmp isrwrapper
isr235:

pushl $235
jmp isrwrapper
isr236:

pushl $236
jmp isrwrapper
isr237:

pushl $237
jmp isrwrapper
isr238:

pushl $238
jmp isrwrapper
isr239:

pushl $239
jmp isrwrapper
isr240:

pushl $240
jmp isrwrapper
isr241:

pushl $241
jmp isrwrapper
isr242:

pushl $242
jmp isrwrapper
isr243:

pushl $243
jmp isrwrapper
isr244:

pushl $244
jmp isrwrapper
isr245:

pushl $245
jmp isrwrapper
isr246:

pushl $246
jmp isrwrapper
isr247:

pushl $247
jmp isrwrapper
isr248:

pushl $248
jmp isrwrapper
isr249:

pushl $249
jmp isrwrapper
isr250:

pushl $250
jmp isrwrapper
isr251:

pushl $251
jmp isrwrapper
isr252:

pushl $252
jmp isrwrapper
isr253:

pushl $253
jmp isrwrapper
isr254:

pushl $254
jmp isrwrapper
isr255:

pushl $255
jmp isrwrapper

isrwrapper:
	# save all registers
  pushl %ebp
  pushl %edi
  pushl %esi
  pushl %edx
  pushl %ecx
  pushl %ebx
  pushl %eax
  pushl %ss
  pushl %gs
  pushl %fs
  pushl %es
  pushl %ds
  movl %cr0, %eax
  pushl %eax
  movl %cr3, %eax
  pushl %eax

  pushl %esp # Save our stack position, so the following call will not mess with it.
  				 # And the other benefit is, we can easy, access the thread state by a pointer.
  call isrcallback # call to our C++ isr handling
  mov %eax, %esp # move our stack pointer to the position of the stack of the new thread.

	# restore all registers
  popl %eax
  movl %eax, %cr3
  popl %eax
  movl %eax, %cr0
  popl %ds
  popl %es
  popl %fs
  popl %gs
  popl %ss
  popl %eax
  popl %ebx
  popl %ecx
  popl %edx
  popl %esi
  popl %edi
  popl %ebp

  cmpl $14,(%esp)
  jne isrwrapper_delete_interrupt
  add $4, %esp
isrwrapper_delete_interrupt:
  # delete interrupt from stack
  add $4, %esp

iret
