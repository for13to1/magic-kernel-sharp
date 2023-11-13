#include "double_image_kernel.h"

typedef struct {
  byte channel;
  double **temp;
  DoubleImage *input;
  DoubleImage *output;
  Kernel *kernel;
  double (*read)(ulong in, void *x);
  void (*write)(ulong out, double v, void *x);
} Pass_;

typedef struct {
  ulong u;
  Pass_ *pass;
} KernelPass_;

static void apply_(Parallel *par);

static double read_x_(ulong in, void *x);

static void write_x_(ulong out, double v, void *x);

static double read_y_(ulong in, void *x);

static void write_y_(ulong out, double v, void *x);


void double_image_new_apply_xy_kernels(
    DoubleImage **p_this,
    DoubleImage *other,
    Kernel *x_kernel,
    Kernel *y_kernel) {
  bool gamma;
  byte channel;
  byte num_channels;
  ulong in_width;
  ulong in_height;
  ulong out_width;
  ulong out_height;
  ulong temp_width;
  ulong temp_height;
  double **_temp = null;
  Pass_ pass;
  DoubleImage *this;

  in_width = other->width;
  in_height = other->height;
  gamma = other->gamma;

  if (x_kernel->nIn != in_width) {
    throw("X kernel n_in (%lu) must equal input image width (%lu)",
        x_kernel->nIn,
        in_width);
  }

  if (y_kernel->nIn != in_height) {
    throw("Y kernel n_in (%lu) must equal input image height (%lu)",
        y_kernel->nIn,
        in_height);
  }

  out_width = x_kernel->nOut;
  out_height = y_kernel->nOut;

  temp_width = out_width;
  temp_height = in_height;

  num_channels = other->numChannels;

  pass.input = other;
  double_image_new(
      p_this,
      out_width,
      out_height,
      num_channels);
  pass.output = this = *p_this;
  newm(&_temp, temp_width, temp_height);
  pass.temp = _temp;

  this->gamma = gamma;

  for (channel = 0; channel < num_channels; channel++) {
    pass.channel = channel;

    pass.kernel = x_kernel;
    pass.read = read_x_;
    pass.write = write_x_;
    parallel_for(
        temp_height,
        x_kernel->minParallel / temp_height,
        apply_,
        &pass);

    pass.kernel = y_kernel;
    pass.read = read_y_;
    pass.write = write_y_;
    parallel_for(
        out_width,
        y_kernel->minParallel / out_width,
        apply_,
        &pass);
  }

  deletemv(&_temp, temp_width);
}

void double_image_new_apply_multiple_xy_kernels(
    DoubleImage **p_this,
    DoubleImage *other,
    ulong num_kernel_pairs,
    Kernel **x_kernels,
    Kernel **y_kernels) {
  byte k;
  DoubleImage *_temp_1 = null;
  DoubleImage *_temp_2 = null;

  if (num_kernel_pairs == 0) {
    throw("Number of kernel pairs cannot be zero");
  }

  for (k = 0; k < num_kernel_pairs; k++) {
    double_image_new_apply_xy_kernels(
        &_temp_2,
        k == 0 ? other : _temp_1,
        x_kernels[k],
        y_kernels[k]);
    double_image_delete(&_temp_1);
    donate(&_temp_2, &_temp_1);
  }

  donate(&_temp_1, p_this);
}


static void apply_(Parallel *par) {
  ulong u;
  KernelPass_ kpass;
  Pass_ *pass;

  pass = par->x;

  kpass.pass = pass;

  for (u = par->start; u < par->end; u++) {
    kpass.u = u;
    kernel_apply(pass->kernel, pass->read, pass->write, &kpass);
  }
}

static double read_x_(ulong in, void *x) {
  byte c;
  ulong i;
  ulong j;
  KernelPass_ *kpass;

  kpass = x;

  c = kpass->pass->channel;
  i = in;
  j = kpass->u;

  return kpass->pass->input->_channels[c][i][j];
}

static void write_x_(ulong out, double v, void *x) {
  ulong i;
  ulong j;
  KernelPass_ *kpass;

  kpass = x;

  i = out;
  j = kpass->u;

  kpass->pass->temp[i][j] = v;
}

static double read_y_(ulong in, void *x) {
  ulong i;
  ulong j;
  KernelPass_ *kpass;

  kpass = x;

  i = kpass->u;
  j = in;

  return kpass->pass->temp[i][j];
}

static void write_y_(ulong out, double v, void *x) {
  byte c;
  ulong i;
  ulong j;
  KernelPass_ *kpass;

  kpass = x;

  c = kpass->pass->channel;
  i = kpass->u;
  j = out;

  kpass->pass->output->_channels[c][i][j] = v;
}
