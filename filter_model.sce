clear all;
close();
// Configure script-- ----------------------------------------------------------

// Which input it uses
// Choose ONE
//signal = "sinus sum";
//signal = "impulse";
//signal = "square";
//signal = "constant";
//signal = "modulo8"
//signal = "sin_random"
signal ="step_signal"

// Which filter it uses
// Choose One
coefficients = "TP";
//coefficients = "Custom";

// FIlter definition -----------------------------------------------------------

n = 32 ;
if coefficients == "TP" then
    coeffs = hex2dec(['0D' '15' '1F' '2C' '3C' '4D' '61' '75' '8A' '9F' 'B3' 'C5' 'D4' 'E1' 'E9' 'EE' 'EE' 'E9' 'E1' 'D4' 'C5' 'B3' '9F' '8A' '75' '61' '4D' '3C' '2C' '1F' '15' '0D' ])
elseif coefficients == "Custom" then
    ft = 'lp' ; 
    fl = %pi  ; 
    coeffs = -128*ffilt(ft,n,fl);
else // DEFAULT
    coeffs = hex2dec(['0D' '15' '1F' '2C' '3C' '4D' '61' '75' '8A' '9F' 'B3' 'C5' 'D4' 'E1' 'E9' 'EE' 'EE' 'E9' 'E1' 'D4' 'C5' 'B3' '9F' '8A' '75' '61' '4D' '3C' '2C' '1F' '15' '0D' ])
end

size_filter = size( coeffs , 2 );

// Calculates Input Signals-----------------------------------------------------
tmax = 2000 ;
Tper = tmax/5;
step = 1 ;
Amp = 110 ;
t = 0:step:tmax-1 ;

func1 = round(   Amp     *  ( sin(     2*%pi*t/Tper    )  + 1 )  );
func2 = round( (Amp/12)  *  ( sin(  2*%pi*t/(Tper/20)  )  + 1 )  );

random_func2 = rand(1, tmax)*35;

// Input 1
sinus_sum = func1 + func2;

// Input 2
impulse = [ 1 zeros( 1 , tmax -1 ) ];

square_signal = [ ones( 1 , 32 ) zeros( 1 , 32 ) ];
for i=1:30
    square_signal = [ square_signal [ ones( 1 , 32 ) zeros( 1 , 32 ) ] ];
end

// Input 3
square_signal = [ square_signal ones( 1 , tmax - size( square_signal , 2 ) ) ];

// Input 4
cts = Amp * ones( 1 , tmax );

//input 5
modulo8_signal = t;
for i=1:tmax
    modulo8_signal(i) = (256 / 8)*modulo(i,9);
    if modulo8_signal(i) == 256 then  
        modulo8_signal(i) = 255;
    end
end

// Input 6
sin_random = func1 + random_func2

//input 7
step_signal = t;
for i=1:tmax
    j = modulo(i,17);
    if(j > 12)
        step_signal(i) = 255 - 16*(j);
    elseif(j < 4)
        step_signal(i) = 16*(16 - j);
    else
        step_signal(i) = 255 - 16*j;
    end;
end


// Selects one of the inputs
if     signal == "sinus sum" then
    input_signal = sinus_sum;
elseif signal == "impulse" then
    input_signal = impulse;
elseif signal == "square" then
    input_signal = square_signal;
elseif signal == "constant" then
    input_signal = cts;
elseif signal == "modulo8"
    input_signal = modulo8_signal
elseif signal == "sin_random"
    input_signal = sin_random
elseif signal == "step_signal"
    input_signal = step_signal
else // then // DEFAULT
    input_signal = impulse;
end

// Filter process --------------------------------------------------------------

padding = [ 1 zeros( 1 , n ) ]; // Filter FIR fixed

output_signal = filter( coeffs , padding , input_signal ) ;

// Plot Input/Output -----------------------------------------------------------

// Global Vision
plot_global = "1"
if plot_global == "1" then
    
    figure()
    subplot( 2 , 1 , 1 );
    plot( t , input_signal , '-b') ;
    title("Input signal complete");
    set(gca(),"grid",[1 1]);
    
    subplot( 2 , 1 , 2 );
    plot( t , output_signal , '-r' ) ;
    title("Output signal global");
    set(gca(),"grid",[1 1]);
end


// OUTPUT TO file --------------------------------------------------------------
// output_file = "Fixed"; //""Fixed"
output_file = "Fixed"; //""Fixed"

if output_file == "Floating"
    
    // EXPORTING THE COEFFS
    N = size(coeffs)(2);
    fid = mopen('/home/noureddine-as/PFE_2018/software/TP_3A_Phelma/examples/serial_filter_demo_v0/include/FIR_COEFFS.h','w');
    mfprintf(fid,'// Number of filter coeffs \n #define    N     %d\n\n', N);
    //mfprintf(fid, '// Frequence d''echantillonnage Fs = %d  \n\n', Fs);
    mfprintf(fid,'float h[N] = {');
    //mfprintf(fid,']= {');
    for i = 1:N-1
        mfprintf(fid,'%f,',coeffs(i));
        if modulo(i,8) == 0    mfprintf(fid, '\n\t\t');
        end
    end
    mfprintf(fid,'%f };\n', coeffs(N));
    mclose(fid);
    
    // EXPORTING INPUT SIGNAL
    N = size(input_signal)(2);
    fid = mopen('/home/noureddine-as/PFE_2018/software/TP_3A_Phelma/examples/serial_filter_demo_v0/include/INPUT_SIGNAL.h','w');
    mfprintf(fid,'// Number of input signal samples \n #define    Nx     %d\n\n', N);
    //mfprintf(fid, '// Frequence d''echantillonnage Fs = %d  \n\n', Fs);
    mfprintf(fid,'float x_signal[Nx] = {');
    //mfprintf(fid,']= {');
    for i = 1:N-1
        mfprintf(fid,'%f,',input_signal(i));
        if modulo(i,8) == 0    mfprintf(fid, '\n\t\t');
        end
    end
    mfprintf(fid,'%f };\n', input_signal(N));
    mclose(fid);
    
    // EXPORTING OUTPUT SIGNAL FOR COMPARISON
    N = size(output_signal)(2);
    fid = mopen('/home/noureddine-as/PFE_2018/software/TP_3A_Phelma/examples/serial_filter_demo_v0/include/OUTPUT_SIGNAL.h','w');
    mfprintf(fid,'// Number of input signal samples \n #define    Ny     %d\n\n', N);
    //mfprintf(fid, '// Frequence d''echantillonnage Fs = %d  \n\n', Fs);
    mfprintf(fid,'float y_signal[Ny] = {');
    //mfprintf(fid,']= {');
    for i = 1:N-1
        mfprintf(fid,'%f,',output_signal(i));
        if modulo(i,8) == 0    mfprintf(fid, '\n\t\t');
        end
    end
    mfprintf(fid,'%f };\n', output_signal(N));
    mclose(fid);

elseif output_file == "Fixed"  // FIXED-POINT
    
    COEFF_PATH = "include/FIR_COEFFS.h"
    OSCILLO_PATH = "src/oscillo_solution.s"
    OUTPUT_BYTES = 1
    
    // EXPORTING THE COEFFS
    N = size(coeffs)(2);
    fid = mopen(COEFF_PATH,'w');
    mfprintf(fid,'// Number of filter coeffs \n #define    N     %d\n\n', N);
    mfprintf(fid,'uint8_t h[N] = {');
    for i = 1:N-1
        mfprintf(fid,'0x%02x,',coeffs(i));
        if modulo(i,16) == 0    mfprintf(fid, '\n\t\t');
        end
    end
    mfprintf(fid,'0x%02x };\n', coeffs(N));
    mclose(fid);
    
    
    Nx = size(input_signal)(2);
    fid = mopen(OSCILLO_PATH,'w');
    mfprintf(fid, '# This is the assembly file generated for INPUT signal \n');
    mfprintf(fid, '  .section "".oscillo"",""aw"",@progbits \n');
    mfprintf(fid, '# Input signal samples \n#define    Nx     %d\n\n', Nx);
    mfprintf(fid, 'app_status: \n  .byte 0x03\nspike_status:\n  .byte 0x00\nin_n_rows:\n  .half 0x%04x \nin_data: \n', Nx);
  
    for i = 1:Nx
        mfprintf(fid,'  .byte 0x%02x\n',input_signal(i));
    end
    
    Ny = size(output_signal)(2);
    mfprintf(fid, '# Output signal samples \n#define    Ny     %d\n\n', Ny);
    mfprintf(fid, '\nout_n_rows:\n  .half 0x%04x \nout_data: \n', Ny);
  
    if(OUTPUT_BYTES == 0) then
        // OUTPUT_BYTES = 0, All Zeros;
        for i = 1:Ny
            mfprintf(fid,'  .byte 0x%02x\n', 0); //output_signal(i) / (2**(16)) );
        end
    end
    if (OUTPUT_BYTES == 1) then
          // OUTPUT_BYTES = 1;
        for i = 1:Ny
            mfprintf(fid,'  .byte 0x%02x\n', bin2dec(dec2bin(uint64(output_signal(i)/ (2**(12))) , 64)) );
        end
    end
    if (OUTPUT_BYTES == 2) then
          // OUTPUT_BYTES = 1;
        for i = 1:Ny
            mfprintf(fid,'  .byte 0x%02x\n',output_signal(i) / (2**(16)) );
        end
    end
  
    mclose(fid);

end
