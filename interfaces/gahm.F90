! MIT License
!
! Copyright (c) 2020 ADCIRC Development Group
!
! Permission is hereby granted, free of charge, to any person obtaining a copy
! of this software and associated documentation files (the "Software"), to deal
! in the Software without restriction, including without limitation the rights
! to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
! copies of the Software, and to permit persons to whom the Software is
! furnished to do so, subject to the following conditions:
!
! The above copyright notice and this permission notice shall be included in
! all copies or substantial portions of the Software.
!
! THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
! IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
! FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
! AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
! LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
! OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
! SOFTWARE.
!
! Author: Zach Cobell
! Contact: zcobell@thewaterinstitute.org
!
module gahm_module
    use, intrinsic :: iso_c_binding, only : c_ptr, c_double, c_int, c_bool, c_long, c_char, c_null_char, c_null_ptr
    implicit none

    type :: gahm_t
        type(c_ptr), private :: ptr = c_null_ptr
    contains
        final :: gahm_destroy
        procedure, pass(this) :: initialize => gahm_initialize
        procedure, pass(this) :: get => gahm_get
    end type gahm_t

    type :: date_t
        integer, private :: m_year, m_month, m_day, m_hour, m_minute, m_second
        integer(c_long), private :: m_serial_date
    contains
        procedure, pass(this) :: set => date_set
        procedure, pass(this) :: serial => date_serial
        procedure, pass(this) :: add => date_add
    end type date_t

    interface
        type(c_ptr) function c_gahm_create(filename, size, xpoints, ypoints) bind(c, name = "gahm_create_ftn")
            use, intrinsic :: iso_c_binding, only : c_char, c_long, c_double, c_ptr
            implicit none
            character(kind = c_char), intent(in) :: filename
            integer(c_long), intent(in), value :: size
            real(c_double), intent(in) :: xpoints(*)
            real(c_double), intent(in) :: ypoints(*)
        end function c_gahm_create

        subroutine c_gahm_delete(ptr) bind(c, name = "gahm_delete_ftn")
            use, intrinsic :: iso_c_binding, only : c_ptr
            implicit none
            type(c_ptr), intent(in), value :: ptr
        end subroutine c_gahm_delete

        subroutine c_gahm_get(ptr, year, month, day, hour, minute, second, size, u, v, p) bind(c, name = "gahm_get_ftn")
            use, intrinsic :: iso_c_binding, only : c_int, c_ptr, c_double, c_long
            implicit none
            type(c_ptr), value :: ptr
            integer(c_int), value :: year, month, day, hour, minute, second
            integer(c_long), value :: size
            real(c_double), intent(inout) :: u(*), v(*), p(*)
        end subroutine c_gahm_get

        integer(c_long) function c_gahm_get_serial_date(year, month, day, hour, minute, second) &
                bind(c, name = "gahm_get_serial_date")
            use, intrinsic :: iso_c_binding, only : c_long
            integer, intent(in), value :: year, month, day, hour, minute, second
        end function c_gahm_get_serial_date

        subroutine c_gahm_date_add(&
                year_in, month_in, day_in, hour_in, minute_in, second_in, &
                add_seconds, &
                year_out, month_out, day_out, hour_out, minute_out, second_out) &
                bind(c, name = "gahm_date_add")
            use, intrinsic :: iso_c_binding, only : c_int
            integer(c_int), intent(in), value :: year_in, month_in, day_in, hour_in, minute_in, second_in, add_seconds
            integer(c_int), intent(out) :: year_out, month_out, day_out, hour_out, minute_out, second_out
        end subroutine c_gahm_date_add

    end interface

contains

    subroutine date_set(this, year, month, day, hour, minute, second)
        implicit none
        class(date_t), intent(inout) :: this
        integer, intent(in) :: year
        integer, intent(in), optional :: month, day, hour, minute, second

        if(present(month))then
            this%m_month = month
        else
            this%m_month = 1
        end if

        if(present(day))then
            this%m_day = day
        else
            this%m_day = 1
        end if

        if(present(hour))then
            this%m_hour = hour
        else
            this%m_hour = 0
        end if

        if(present(minute))then
            this%m_minute = minute
        else
            this%m_minute = 0
        end if

        if(present(second))then
            this%m_second = second
        else
            this%m_second = 0
        end if

        this%m_year = year

        this%m_serial_date = c_gahm_get_serial_date(this%m_year, this%m_month, this%m_day, &
                this%m_hour, this%m_minute, this%m_second)

    end subroutine date_set

    integer(8) function date_serial(this)
        implicit none
        class(date_t), intent(in) :: this
        date_serial = this%m_serial_date
    end function date_serial

    type(date_t) function date_add(this, seconds)
        implicit none
        class(date_t), intent(in) :: this
        integer, intent(in) :: seconds
        call c_gahm_date_add(this%m_year, this%m_month, this%m_day, this%m_hour, this%m_minute, this%m_second, &
                seconds, &
                date_add%m_year, date_add%m_month, date_add%m_day, date_add%m_hour, date_add%m_minute, date_add%m_second)
    end function date_add

    subroutine gahm_initialize(this, filename, size, xpoints, ypoints)
        implicit none
        class(gahm_t), intent(inout) :: this
        character(*), intent(in) :: filename
        integer(c_long), intent(in) :: size
        real(c_double), intent(in) :: xpoints(*)
        real(c_double), intent(in) :: ypoints(*)
        this%ptr = c_gahm_create(TRIM(filename) // c_null_char, size, xpoints, ypoints)
    end subroutine gahm_initialize

    subroutine gahm_get(this, date, size, u, v, p)
        implicit none
        class(gahm_t), intent(in) :: this
        class(date_t), intent(in) :: date
        integer(c_long), intent(in) :: size
        real(c_double), intent(inout) :: u(*), v(*), p(*)
        call c_gahm_get(this%ptr, date%m_year, date%m_month, &
                date%m_day, date%m_hour, date%m_minute, date%m_second, size, u, v, p)
    end subroutine gahm_get

    subroutine gahm_destroy(this)
        implicit none
        type(gahm_t), intent(in) :: this
        call c_gahm_delete(this%ptr)
    end subroutine gahm_destroy

end module gahm_module
