   a��  u�  Qb�f    6569E�c        �   $S�Pd   	   push.1109   a��  ��  QbT    1109E�c        �   $S�Pd   	   push.4424   a��  d�  Qb E    4424E�c        �   `   DI]Dc     @    5I]q�������	�D�8            !  �!  �-��{�2�-��y	����|
� ���  5�� m�� 5�� �
�� 5�� ��� 5�� ^��	 5�� Q ��
 5�� �#�� 5�� � �� 5�� ��� 5�� ���	 5�� F��
 5�� ��� 5��! U�� 5��# H�� 5��%�6��'^���)ĩ  Rb�`          Ib����    �� ,�i+      0��9����������    )c            







�A�Eo��   h���      �	�WdQ/ �  �(;�L8���� �~����h��?p��G��A�Eo��   �4	�                                                                                                                                                                                                                                                                                                                           
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifndef FsNew_h
#define FsNew_h
#include <stddef.h>
#include <stdint.h>

/** 32-bit alignment */
typedef uint32_t newalign_t;

/** Size required for exFAT or FAT class. */
#define FS_SIZE(etype, ftype) \
  (sizeof(ftype) < sizeof(etype) ? sizeof(etype) : sizeof(ftype))

/** Dimension of aligned area. */
#define NEW_ALIGN_DIM(n) \
  (((size_t)(n) + sizeof(newalign_t) - 1U)/sizeof(newalign_t))

/** Dimension of aligned area for etype or ftype class. */
#define FS_ALIGN_DIM(etype, ftype) NEW_ALIGN_DIM(FS_SIZE(etype, ftype))

/** Custom new placement operator */
void* operator new(size_t size, newalign_t* ptr);
#endif  // FsNew_h
