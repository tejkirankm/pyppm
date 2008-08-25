from arith_codec import ArithCodec

class ArithEncoder(ArithCodec):

    def __init__(self, out):
        ArithCodec.__init__(self)
        self.out = out
        self.bits_to_follow = 0

    def close(self):
        """\
        Close the encoder and flush all buffered bits.
        """
        self.bits_to_follow += 1        # needs a final bit, not sure why
        if self.low < self.FIRST_QUARTER:
            self.write_false()
        else:
            self.write_true()

    def encode(self, low_count, high_count, total_count):
        """\
        Encode an interval.

         - low_count: the cumulative count of symbol below this one
         - high_count: the cumulative count of symbol for this one and below
         - total_count: the cumulative count of all symbols
        """
        rng = self.high-self.low

        self.high = self.low + rng*high_count/total_count
        self.low =  self.low + rng*low_count/total_count

        from sys import stdin
        while True:
            print '%10d, %10d, %10d' % (self.low, self.HALF, self.high)
            print '%.4f, %.4f' % (float(self.low)/self.TOP_VALUE, float(self.high)/self.TOP_VALUE)
            print
            
            if self.high < self.HALF:
                self.write_false()

            elif self.approximate_ge(self.low, self.HALF):
                self.write_true()
                self.low -= self.HALF
                self.high -= self.HALF
                
            elif self.approximate_ge(self.low, self.FIRST_QUARTER) and \
                     self.high < self.THIRD_QUARTER:
                self.bits_to_follow += 1
                
                self.low -= self.FIRST_QUARTER
                self.high -= self.FIRST_QUARTER
                
            else:
                break

            self.low = self.low << 1
            self.high = self.high << 1


    def write_true(self):
        """\
        Write a '1' bit and follow with it bits_to_follow '0' bits.
        """
        self.out.write_true()
        self.out.write_false_n(self.bits_to_follow)
        self.bits_to_follow = 0

    def write_false(self):
        """\
        Write a '0' bit and follow with it bits_to_follow '1' bits.
        """
        self.out.write_false()
        self.out.write_true_n(self.bits_to_follow)
        self.bits_to_follow = 0
    
