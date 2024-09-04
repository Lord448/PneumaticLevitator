class PID:
    def __init__(self, kp : float, 
                 ki : float, 
                 kd : float, 
                 setPoint : float):
        self._kp = kp
        self._ki = ki
        self._kd = kd
        self._setpoint = setPoint
        self._range = range
        self._error = 0
        self._pastError = 0
        self._p = 0
        self._i = 0
        self._d = 0
        self._offset = 70
    
    def process(self, input : float) -> float:
        #Normalizing on specific case
        MAX_DISTANCE = 530
        setPointNormalized = (self._setpoint *100) / MAX_DISTANCE
        input = (input * 100) / MAX_DISTANCE
        self._error = setPointNormalized - input
        self._p = self._error * self._kp
        self._i = self._i + (self._error * self._ki)
        self._d = (self._error - self._pastError) * self._kd
        self._pastError = self._error
        #TODO Debug
        print(f"Error: {self._error}")
        #print(f"SP: {self._setpoint}")
        #print(f"Input: {input}")
        #print(f"P: {self._p}")
        #print(f"I: {self._i}")
        #print(f"D: {self._d}")
        #Limits on I
        if self._i > 30:
            self._i = 30
        elif self._i < -30:
            self._i = -30
        result = self._p + self._i + self._d + self._offset
        if result > 100:
            result = 100
        elif result < 0:
            result = 0
        return result
    
    
    