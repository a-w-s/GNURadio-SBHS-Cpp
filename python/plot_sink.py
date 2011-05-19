
from gnuradio import gr
import threading
import numpy
import matplotsink
import Queue

class _plot_sink_base(gr.hier_block2, threading.Thread):
	"""
	The thread polls the message queue for values and writes to matplotsink callback
	"""
	def __init__(self, parent, title, bgcolor, linecolor):		
		self._parent = parent
		self._title = title
		
		print "Initing block: %s" % title

		self.plotQueue = Queue.Queue()

		self.win = matplotsink.matplotsink(parent,title, self.plotQueue, bgcolor, linecolor)		

		self._item_size = self._size*1
		#init hier block
		gr.hier_block2.__init__(
			self, 'plot_sink',
			gr.io_signature(1, 1, self._item_size),
			gr.io_signature(0, 0, 0),
		)
		#create blocks		
		self._msgq = gr.msg_queue(2)
		message_sink = gr.message_sink(self._item_size, self._msgq, False)
		#connect
		self.connect(self, message_sink)
		#setup thread
		threading.Thread.__init__(self)
		self.setDaemon(True)
		self.start() 

	def run(self):
		while True:
			msg = self._msgq.delete_head().to_string()[-self._item_size:]
			arr = map(self._cast, numpy.fromstring(msg, self._numpy))
			print "Sending value: [%.1f]" %(arr[0])
			self.plotQueue.put(arr[0])

class plot_sink_b(_plot_sink_base): _numpy, _size, _cast = numpy.int8, gr.sizeof_char, int
class plot_sink_s(_plot_sink_base): _numpy, _size, _cast = numpy.int16, gr.sizeof_short, int
class plot_sink_i(_plot_sink_base): _numpy, _size, _cast = numpy.int32, gr.sizeof_int, int
class plot_sink_f(_plot_sink_base): _numpy, _size, _cast = numpy.float32, gr.sizeof_float, float
class plot_sink_c(_plot_sink_base): _numpy, _size, _cast = numpy.complex64, gr.sizeof_gr_complex, complex
