! GNU General Public License v3.0
!
! This file is part of the GAHM model (https://github.com/adcirc/gahm).
! Copyright (c) 2023 ADCIRC Development Group.
!
! This program is free software: you can redistribute it and/or modify
! it under the terms of the GNU General Public License as published by
! the Free Software Foundation, version 3.
!
! This program is distributed in the hope that it will be useful, but
! WITHOUT ANY WARRANTY; without even the implied warranty of
! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
! General Public License for more details.
!
! You should have received a copy of the GNU General Public License
! along with this program. If not, see <http://www.gnu.org/licenses/>.
!
! Author: Zach Cobell
! Contact: zcobell@thewaterinstitute.org
!
module gahm_module
    use, intrinsic :: iso_c_binding, only: c_double, c_int, c_bool, c_long, c_char, c_null_char, c_null_ptr

        type :: gahm_t
            integer(c_long), private  :: ptr = -1

            contains
                final :: gahm_destroy
                procedure, pass(this) :: initialize => gahm_initialize
                procedure, pass(this) :: get => gahm_get

        end type gahm_t

        type :: date_t
            integer, private :: m_year, m_month, m_day, m_hour, m_minute, m_second
            integer(c_long)  :: m_serial_date

            contains

            procedure, pass(this) :: set => date_set
            procedure, pass(this) :: serial => date_serial
            procedure, pass(this) :: add => date_add

        end type date_t

        interface
            integer(c_long) function c_gahm_create(filename, size, xpoints, ypoints, quiet) bind(c, name="gahm_create_ftn")
                use, intrinsic :: iso_c_binding, only: c_char, c_long, c_double, c_bool
                implicit none
                character(kind=c_char), intent(in) :: filename
                integer(c_long), intent(in), value :: size
                real(c_double), intent(in)         :: xpoints(*)
                real(c_double), intent(in)         :: ypoints(*)
                logical(c_bool), intent(in), value :: quiet
            end function c_gahm_create

            subroutine c_gahm_destroy(gahm) bind(c, name="gahm_destroy_ftn")
                use, intrinsic :: iso_c_binding, only: c_long
                implicit none
                integer(c_long), intent(in), value :: gahm
            end subroutine c_gahm_destroy

            subroutine c_gahm_get(ptr, year, month, day, hour, minute, second, size, u, v, p) bind(c, name="gahm_get_ftn")
                use iso_c_binding, only: c_long, c_int, c_double
                implicit none
                integer(c_long), intent(in), value :: ptr
                integer(c_int), intent(in), value  :: year, month, day, hour, minute, second
                integer(c_long), intent(in), value :: size
                real(c_double), intent(inout)      :: u(*), v(*), p(*)
            end subroutine c_gahm_get

            integer(c_long) function c_gahm_get_serial_date(year, month, day, hour, minute, second) &
                                        bind(c, name="gahm_get_serial_date_ftn") result(serial_date)
                use iso_c_binding, only: c_int, c_double, c_long
                implicit none
                integer(c_int), intent(in), value :: year, month, day, hour, minute, second
            end function c_gahm_get_serial_date

            subroutine c_gahm_date_add(year_in, month_in, day_in, hour_in, minute_in, second_in, seconds, &
                                       year_out, month_out, day_out, hour_out, minute_out, second_out) &
                                        bind(c, name="gahm_date_add_ftn")
                use iso_c_binding, only: c_int, c_double, c_long
                implicit none
                integer(c_long), intent(in), value :: seconds
                integer(c_int), intent(in), value  :: year_in, month_in, day_in, hour_in, minute_in, second_in
                integer(c_int), intent(out)        :: year_out, month_out, day_out, hour_out, minute_out, second_out
            end subroutine c_gahm_date_add

        end interface

    contains

        subroutine date_set(this, year, month, day, hour, minute, second)
            implicit none
            class(date_t), intent(inout)  :: this
            integer, intent(in)           :: year
            integer, intent(in), optional :: month, day, hour, minute, second

            this%m_year = year
            this%m_month = 1
            this%m_day = 1
            this%m_hour = 0
            this%m_minute = 0
            this%m_second = 0

            if (present(month))this%m_month = month
            if (present(day))this%m_day = day
            if (present(hour))this%m_hour = hour
            if (present(minute))this%m_minute = minute
            if (present(second))this%m_second = second

            this%m_serial_date = c_gahm_get_serial_date(this%m_year, this%m_month, this%m_day, &
                                                        this%m_hour, this%m_minute, this%m_second)

        end subroutine date_set

        integer(8) function date_serial(this)
            implicit none
            class(date_t), intent(in) :: this
            date_serial = this%m_serial_date
        end function date_serial

        type(date_t) function date_add(this, seconds) result(out_date)
            implicit none
            class(date_t), intent(inout) :: this
            integer(8), intent(in)       :: seconds
            integer                      :: out_year, out_month, out_day, out_hour, out_minute, out_second

            call c_gahm_date_add(this%m_year, this%m_month, this%m_day, this%m_hour, this%m_minute, &
                 this%m_second, seconds, out_year, out_month, out_day, out_hour, out_minute, out_second)

            out_date%m_year = out_year
            out_date%m_month = out_month
            out_date%m_day = out_day
            out_date%m_hour = out_hour
            out_date%m_minute = out_minute
            out_date%m_second = out_second

        end function date_add

        subroutine gahm_initialize(this, filename, size, xpoints, ypoints)
            implicit none
            class(gahm_t), intent(inout) :: this
            character(len=*), intent(in) :: filename
            integer(c_long), intent(in)  :: size
            real(c_double), intent(in)   :: xpoints(*), ypoints(*)
            logical(c_bool)              :: quiet = .true.
            this%ptr = c_gahm_create(trim(filename)//c_null_char, size, xpoints, ypoints, quiet)
        end subroutine gahm_initialize

        subroutine gahm_destroy(this)
            implicit none
            type(gahm_t), intent(in) :: this
            call c_gahm_destroy(this%ptr)
        end subroutine gahm_destroy

        subroutine gahm_get(this, date, size, u, v, p)
            implicit none
            class(gahm_t), intent(inout)   :: this
            class(date_t), intent(in)      :: date
            integer(c_long), intent(in)    :: size
            real(c_double), intent(inout)  :: u(*), v(*), p(*)
            call c_gahm_get(this%ptr, date%m_year, date%m_month, date%m_day, &
                    date%m_hour, date%m_minute, date%m_second, size, u, v, p)
        end subroutine gahm_get

end module GAHM_MODULE