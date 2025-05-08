// ---------------------------------------------------------------------------------------------------------------------------------------------------->
// THIS IS WHERE YOU SPECIFY THE SETTINGS FOR YOUR LIGHTS
// ---------------------------------------------------------------------------------------------------------------------------------------------------->

    // SETUP INSTRUCTIONS

    // EXPLANATION OF STATES
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
    // Each light can respond to multiple different conditions, or States. These are:
    // - Channel 3 - Position 1
    // - Channel 3 - Position 2
    // - Channel 3 - Position 3    (This is the middle position if using a 3-position switch)
    // - Channel 3 - Position 4
    // - Channel 3 - Position 5    (This is position 3 if using a 3-position switch, or position 2 if using a 2-position switch)
    // - Forward
    // - Reverse
    // - Stop
    // - Stop Delay                (stopped for LongStopTime_mS milliseconds, set by user on UserConfig tab)
    // - Brake
    // - Right Turn
    // - Left Turn
    // - No Turn
    // - Accelerating - this is a subset of the Forward state, and occurs during heavy acceleration (defined in UserConfig)
    // - Decelerating - this is a subset of the Forward state, and occurs during deceleration (defined in UserConfig)
    //
    // State Notes:
    // - At the very least, you must plug in the Throttle channel. Steering and Channel 3 are optional
    // - If you do not plug in a steering channel, then obviously you will never encounter the Right, Left and No Turn states. If you don't want to use
    //   the steering wheel but have another 3 position switch on your transmitter, you can plug that channel into the steering input and use the three
    //   states (Left turn, Right turn, No turn) for the 3 switch positions.
    // - If you do not plug in a third channel, the program will only use the values specified in Position 1 as the default. In other words, it acts
    //   as if you have a one-position switch that is always in Position 1
    // - If your third channel is only a 2-position switch, the program will switch between Position 1 and Position 5
    // - If your third channel is a 3-position switch, you will be able to switch between Positions 1, 3, and 5
    // - If your third channel is a 5-position switch, you will be able to select all five Positions
    // - For the third channel to operate correctly, and in fact, for all channels to operate correctly, you need to run through the Radio Setup once.

    // EXPLANATION OF SETTINGS
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
    // For each light, you need to specify what will happen in each state - this is called the setting. The possible settings are:
    // - ON
    // - OFF
    // - FADEON         * Fades on slowly, time set in AA_UserConfig.h (FadeInTime)
    // - FADEOFF        * Fades off slowly, time set in AA_UserConfig.h (FadeOutTime)
    // - NA
    // - BLINK            Will blink at the rate set in AA_UserConfig.h (BlinkInterval)
    // - BLINK_ALT        Same as BLINK but alternating
    // - FASTBLINK        Will blink at the fast rate set in AA_UserConfig.h (FastBlinkInterval)
    // - FASTBLINK_ALT    Same as FASTBLINK but alternating    
    // - SOFTBLINK        A blink that recreates the look of old incandescant light bulbs. The timing of this blink is not user adjustable.
    // - DIM            * Will dim to the level set in AA_UserConfig.h (DimLevel)
    // - XENON          * Turns a light on but with a special effect that looks like a xenon bulb turning on
    // - BACKFIRE         Special effect that blinks a light randomly for a short period of time (use this under the Decelerating state for tailpipe/muffler LEDs)
    //
    // Settings Notes:
    // - For the positions, you probably don't want any of them to be NA. they should be ON, OFF, whatever.
    // - The opposite is true for the other states - if you don't want anything to happen, set it to NA.
    // * Some settings require a special feature known as PWM. These are marked above with an asterisks (*). Not all of the lights on the board are capable of implementing PWM,
    //   only the first 6 sockets. If you look at the underside of the physical board, these lights are marked with an asterisks (*). If you want to use these special settings,
    //   they must be on lights 1-6. Otherwise if you specify one of these settings on lights 7 or 8, the program will simply turn them OFF isntead.

    // EXPLANATION OF SCHEMES
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
    // A Scheme defines every setting for every light in every state. You may have as many Schemes as code space allows. Two are provided below to start,
    // if you want more, add them below, but remember to also update the NumSchemes variable at the very top of AA_UserConfig.h

    // HOW TO SETUP YOUR LIGHTS
    // ------------------------------------------------------------------------------------------------------------------------------------------------>
    // Below you will see the lighting schemes. Each Scheme has a single row for each of the eight lights. The columns represent the states. The values
    // in the individual tables represent the settings for that light at that state.
    //
    // OK, YOU'RE READY. TRY NOT TO MESS UP THE LAYOUT. JUST CHANGE THE SETTINGS.

    const PROGMEM uint8_t Schemes[NumSchemes][NumLights][NumStates] =
    {
        {
        //                                     IF CHANNEL 3 is only 3-position switch, values in Pos2 and Pos4 will be ignored (just use Pos1, Pos3, Pos5)
        //     SCHEME ONE - EXAMPLE PROVIDED
        //     Pos 1 *       Pos 2         Pos 3 *       Pos 4         Pos 5 *       Forward       Reverse       Stop          StopDelay     Brake         Right Turn    Left Turn     No Turn       Accelerating  Decelerating
        // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            {  OFF,          OFF    ,      ON,           ON,           ON,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA           },  // Light 1    -- Headlight 
            {  OFF,          OFF,          FADEOFF,      XENON,        XENON,        NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA           },  // Light 2    -- Light Bar 
            {  OFF,          OFF,          DIM,          DIM,          DIM,          NA,           NA,           ON,           DIM,          ON,           NA,           NA,           NA,           NA,           FADEON       },  // Light 3    -- Brake Light 
            {  SOFTBLINK,    OFF,          DIM,          DIM,          DIM,          NA,           NA,           NA,           NA,           NA,           BLINK,        NA,           NA,           NA,           NA           },  // Light 4    -- Right Turn Lights 
            {  SOFTBLINK,    OFF,          DIM,          DIM,          DIM,          NA,           NA,           NA,           NA,           NA,           NA,           BLINK,        NA,           NA,           NA           },  // Light 5    -- Left Turn Lights 
            {  OFF,          OFF,          OFF,          OFF,          OFF,          NA,           ON,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA           },  // Light 6    -- Reverse Lights 
            {  ON,           OFF,          OFF,          OFF,          OFF,          NA,           NA,           NA,           ON,           NA,           NA,           NA,           NA,           NA,           NA           },  // Light 7 *  -- Cab lights 
            {  OFF,          OFF,          OFF,          OFF,          ON,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA           }   // Light 8 *  -- Rock lights 
        },
        {
        //     SCHEME TWO - BLANK
        //     Pos 1         Pos 2         Pos 3         Pos 4         Pos 5         Forward       Reverse       Stop          StopDelay     Brake         Right Turn    Left Turn     No Turn       Accelerating  Decelerating
        // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            {  BACKFIRE,     SOFTBLINK,    XENON,        BLINK_ALT,    FASTBLINK_ALT,NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA           },  // Light 1    -- 
            {  BACKFIRE,     SOFTBLINK,    XENON,        BLINK,        FASTBLINK,    NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA           },  // Light 2    -- 
            {  BACKFIRE,     SOFTBLINK,    XENON,        BLINK_ALT,    FASTBLINK_ALT,NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA           },  // Light 3    -- 
            {  BACKFIRE,     SOFTBLINK,    XENON,        BLINK,        FASTBLINK,    NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA           },  // Light 4    -- 
            {  BACKFIRE,     SOFTBLINK,    XENON,        BLINK_ALT,    FASTBLINK_ALT,NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA           },  // Light 5    -- 
            {  BACKFIRE,     SOFTBLINK,    XENON,        BLINK,        FASTBLINK,    NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA           },  // Light 6    -- 
            {  BACKFIRE,     SOFTBLINK,    ON,           BLINK_ALT,    FASTBLINK_ALT,NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA           },  // Light 7 *  -- 
            {  BACKFIRE,     SOFTBLINK,    ON,           BLINK,        FASTBLINK,    NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA,           NA           }   // Light 8 *  -- 
        }
    };
