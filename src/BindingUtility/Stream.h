#include "Common.h"
#include <Jolt/Core/StreamIn.h>
#include <Jolt/Core/StreamOut.h>

class PythonStreamOut : public StreamOut {
  public:
    PythonStreamOut(nb::object py_io_object);

    ~PythonStreamOut() override = default;

    void WriteBytes(const void *inData, size_t inNumBytes) override;

    bool IsFailed() const override;

  private:
    nb::object m_py_io;
    bool m_failed = false;
};

class PythonStreamIn : public StreamIn {
  public:
    PythonStreamIn(nb::object py_io_object);

    ~PythonStreamIn() override = default;

    void ReadBytes(void *outData, size_t inNumBytes) override;
    bool IsEOF() const override;
    bool IsFailed() const override;

  private:
    nb::object m_py_io;
    bool m_is_eof = false;
    bool m_is_failed = false;
};