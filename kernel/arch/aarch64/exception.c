#include "aarch64/exception.h"
#include "aarch64/aarch64.h"
#include "common_defs.h"

static inline void print_regs(ExceptionFrame *frame);
static inline void print_exception_class_info(ExceptionFrame *frame);

void exception_handler(ExceptionFrame *frame)
{
	printf("Caught exception:\n");
	print_exception_class_info(frame);
	print_regs(frame);
}

void irq_handler(void)
{
}

void unhandled_exception_handler(ExceptionFrame *frame)
{
	printf("Unhandled exception:\n");
	print_exception_class_info(frame);
	print_regs(frame);
	wfi();
}

static inline void print_regs(ExceptionFrame *frame)
{
	printf("Registers:\n");
#define P(reg, frame) printf("\t" #reg " = %p\n", (frame)->reg)
	P(sp, frame);
	P(SP_EL0, frame);
	P(ELR_EL1, frame);
	P(SPSR_EL1, frame);
	P(ESR_EL1, frame);
	P(FAR_EL1, frame);
	printf("\t----------------------\n");
	P(x0, frame);
	P(x1, frame);
	P(x2, frame);
	P(x3, frame);
	P(x4, frame);
	P(x5, frame);
	P(x6, frame);
	P(x7, frame);
	P(x8, frame);
	P(x9, frame);
	P(x10, frame);
	P(x11, frame);
	P(x12, frame);
	P(x13, frame);
	P(x14, frame);
	P(x15, frame);
	P(x16, frame);
	P(x17, frame);
	P(x19, frame);
	P(x20, frame);
	P(x21, frame);
	P(x22, frame);
	P(x23, frame);
	P(x24, frame);
	P(x25, frame);
	P(x26, frame);
	P(x27, frame);
	P(x28, frame);
	P(x29, frame);
	P(x30, frame);
#undef P
}

static inline const i8 *esr_ec_decode(u64 ec, u64 iss)
{
	// TODO: parse iss value depending on the type of exception class.

	UNUSED_ARG(iss);

	const i8 *description = nullptr;
	switch (ec) {
	case 0b000000:
	default: {
		description = "Unknown reason";
		break;
	}
	case 0b000001:
		description = "Wrapped WF* instruction execution";
		break;

	case 0b000011:
		description = "Trapped MCR or MRC access with coproc=0b1111";
		break;

	case 0b000100:
		description = "Trapped MCRR or MRRC access with coproc=0b1111";
		break;

	case 0b000101:
		description = "Trapped MCR or MRC access with coproc=0b1110";
		break;

	case 0b000110:
		description = "Trapped LDC or STC access";
		break;

	case 0b000111:
		description = "Trapped access to SVE, Advanced SIMD or "
			      "floating point";
		break;

	case 0b001010:
		description = "Trapped execution of an LD64B, ST64B, ST64BV, "
			      "or ST64BV0 instruction";
		break;

	case 0b001100:
		description = "Trapped MRRC access with (coproc==0b1110)";
		break;

	case 0b001101:
		description = "Branch Target Exception";
		break;

	case 0b001110:
		description = "Illegal Execution state";
		break;

	case 0b010001:
		description = "SVC instruction execution in AArch32 state";
		break;

	case 0b010101:
		description = "SVC instruction execution in AArch64 state";
		break;

	case 0b010110:
		description = "HVC instruction execution in AArch64 state";
		break;

	case 0b010111:
		description = "SMC instruction execution in AArch64 state";
		break;

	case 0b011000:
		description = "Trapped MSR, MRS or System instruction "
			      "execution in AArch64 state";
		break;

	case 0b011001:
		description = "Access to SVE functionality trapped as a result "
			      "of CPACR_EL1.ZEN, CPTR_EL2.ZEN, CPTR_EL2.TZ, or "
			      "CPTR_EL3.EZ";
		break;

	case 0b011100:
		description = "Exception from a Pointer Authentication "
			      "instruction authentication failure";
		break;

	case 0b100000:
		description = "Instruction Abort from a lower Exception level";
		break;

	case 0b100001:
		description = "Instruction Abort taken without a change in "
			      "Exception level";
		break;

	case 0b100010:
		description = "PC alignment fault exception";
		break;

	case 0b100100:
		description = "Data Abort from a lower Exception level";
		break;

	case 0b100101:
		description = "Data Abort taken without a change in Exception "
			      "level";
		break;

	case 0b100110:
		description = "SP alignment fault exception";
		break;

	case 0b101000:
		description = "Trapped floating-point exception taken from "
			      "AArch32 state";
		break;

	case 0b101100:
		description = "Trapped floating-point exception taken from "
			      "AArch64 state";
		break;

	case 0b101111:
		description = "SError interrupt";
		break;

	case 0b110000:
		description = "Breakpoint exception from a lower Exception "
			      "level";
		break;

	case 0b110001:
		description = "Breakpoint exception taken without a change in "
			      "Exception level";
		break;

	case 0b110010:
		description = "Software Step exception from a lower Exception "
			      "level";
		break;

	case 0b110011:
		description = "Software Step exception taken without a change "
			      "in Exception level";
		break;

	case 0b110100:
		description = "Watchpoint exception from a lower Exception "
			      "level";
		break;

	case 0b110101:
		description = "Watchpoint exception taken without a change in "
			      "Exception level";
		break;

	case 0b111000:
		description = "BKPT instruction execution in AArch32 state";
		break;

	case 0b111100:
		description = "BRK instruction execution in AArch64 state";
		break;
	}
	return description;
}

static inline const i8 *decode_esr(u64 esr)
{
	u64 ec = EXTRACT_BITS(esr, 31, 26);
	u64 iss = EXTRACT_BITS(esr, 24, 0);
	return esr_ec_decode(ec, iss);
}

static void print_exception_class_info(ExceptionFrame *frame)
{
	const i8 *ec = decode_esr(frame->ESR_EL1);
	printf("\tException class: %s\n", ec);
}
