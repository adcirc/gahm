program gahm_ftn_owi

    call gahm_sample() !...Destructor is not called in main, but is called in subroutines

end program gahm_ftn_owi

subroutine gahm_sample
    use gahm_module, only : gahm_t, date_t
    implicit none

    type(gahm_t) :: gahm_data
    type(date_t) :: start_date
    type(date_t) :: current_date
    real(8), allocatable :: x(:)
    real(8), allocatable :: y(:)
    real(8), allocatable :: u(:)
    real(8), allocatable :: v(:)
    real(8), allocatable :: p(:)
    character(200) :: filename
    integer(8) :: np
    integer :: i

    !...Initialization of GAHM begins
    allocate(x(1))
    allocate(y(1))
    allocate(u(1))
    allocate(v(1))
    allocate(p(1))

    x(1) = -90.0
    y(1) = 29.0

    filename = "../testing/test_files/bal122005.dat"
    np = size(x)
    call gahm_data%initialize(filename, np, x, y)
    call start_date%set(2005, 8, 27)
    !...End initialization of GAHM

    !...Main loop calling GAHM
    do i = 1, 86400, 1800
        current_date = start_date%add(i)
        call gahm_data%get(current_date, np, u, v, p)
        write(*, *) u(1), v(1), p(1)
    end do

end subroutine gahm_sample
