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
module test_criteria
    implicit none

    real(8), parameter :: tol = 1.0e-6

    contains

    logical function is_equal(a, b)
        implicit none
        real(8), intent(in) :: a, b
        is_equal = abs(a - b) <= tol
    end function is_equal

end module test_criteria

program TEST_VortexFortran
    implicit none
    !...Note that we're calling inside a subroutine.
    ! This is because OOP in fortran is a bit different
    ! than in other languages and the cleanup of the
    ! object is not done at the end of a main program,
    ! only when there is a return from another function/subroutine
    call test_001()
end program TEST_VortexFortran

subroutine test_001()
    use test_criteria, only: is_equal
    use gahm_module
    implicit none

    type(gahm_t)                       :: gahm
    type(date_t)                       :: start_date, current_date
    character(200)                     :: filename
    integer                            :: i, j
    integer(8)                         :: n_pts
    real(8),allocatable                :: x(:), y(:), u(:), v(:), p(:)
    logical                            :: exists
    real(8), allocatable               :: solution(:,:)
    real(8),parameter                  :: solution_vec(49*3) = (/&
             -.334456317d0, -1.370004934d0, 1012.182287740d0,&
             -.377050220d0, -1.505479131d0, 1012.099267197d0,&
             -.422991494d0, -1.646868718d0, 1012.012183798d0,&
             -.472419834d0, -1.794151374d0, 1011.920982390d0,&
             -.525475063d0, -1.947294857d0, 1011.825608403d0,&
             -.582296745d0, -2.106256482d0, 1011.726008102d0,&
             -.643023810d0, -2.270982694d0, 1011.622128815d0,&
             -.707794170d0, -2.441408766d0, 1011.513919126d0,&
             -.776744349d0, -2.617458574d0, 1011.401329058d0,&
             -.850009116d0, -2.799044462d0, 1011.284310226d0,&
             -.927721125d0, -2.986067193d0, 1011.162815985d0,&
            -1.010010559d0, -3.178415965d0, 1011.036801564d0,&
            -1.097004796d0, -3.375968503d0, 1010.906224189d0,&
            -1.127511506d0, -3.405984051d0, 1010.885982245d0,&
            -1.158415744d0, -3.435647818d0, 1010.865997537d0,&
            -1.189714059d0, -3.464937719d0, 1010.846281709d0,&
            -1.221402565d0, -3.493831062d0, 1010.826846669d0,&
            -1.253476921d0, -3.522304550d0, 1010.807704583d0,&
            -1.285932297d0, -3.550334266d0, 1010.788867882d0,&
            -1.318763354d0, -3.577895667d0, 1010.770349261d0,&
            -1.351964205d0, -3.604963582d0, 1010.752161682d0,&
            -1.385528390d0, -3.631512202d0, 1010.734318373d0,&
            -1.419448841d0, -3.657515079d0, 1010.716832831d0,&
            -1.453717846d0, -3.682945122d0, 1010.699718821d0,&
            -1.488327018d0, -3.707774591d0, 1010.682990376d0,&
            -1.498815581d0, -3.692755173d0, 1010.718605461d0,&
            -1.509663298d0, -3.678488862d0, 1010.753296710d0,&
            -1.520879781d0, -3.664965419d0, 1010.787090438d0,&
            -1.532474784d0, -3.652174676d0, 1010.820012240d0,&
            -1.544523131d0, -3.640259526d0, 1010.851991707d0,&
            -1.556873587d0, -3.628828889d0, 1010.883290972d0,&
            -1.569642862d0, -3.618125703d0, 1010.913774764d0,&
            -1.582841908d0, -3.608140455d0, 1010.943466270d0,&
            -1.596481892d0, -3.598863706d0, 1010.972388027d0,&
            -1.610574206d0, -3.590286077d0, 1011.000561944d0,&
            -1.625130474d0, -3.582398235d0, 1011.028009321d0,&
            -1.640162555d0, -3.575190882d0, 1011.054750868d0,&
            -1.709574407d0, -3.691310418d0, 1010.970135946d0,&
            -1.781522050d0, -3.810459005d0, 1010.882085749d0,&
            -1.856076163d0, -3.932677078d0, 1010.790466978d0,&
            -1.933304548d0, -4.057995462d0, 1010.695146114d0,&
            -2.013275271d0, -4.186442170d0, 1010.595985293d0,&
            -2.096056587d0, -4.318042211d0, 1010.492842213d0,&
            -2.181716864d0, -4.452817383d0, 1010.385570040d0,&
            -2.270324509d0, -4.590786068d0, 1010.274017321d0,&
            -2.361947890d0, -4.731963030d0, 1010.158027895d0,&
            -2.456655268d0, -4.876359207d0, 1010.037440811d0,&
            -2.554514722d0, -5.023981501d0, 1009.912090250d0,&
            -2.655594079d0, -5.174832576d0, 1009.781805448d0 &
        /)

    !...Reshape into n x 3 array
    solution = transpose(reshape(solution_vec, (/3,49/)))

    !...Initialization of GAHM begins

    !...Allocate memory for 1 point where we will validate the solution
    allocate(x(1))
    allocate(y(1))
    allocate(u(1))
    allocate(v(1))
    allocate(p(1))

    !...Known good solution
    x(1) = -90.0
    y(1) = 29.0

    filename = "../tests/test_files/bal122005.dat"

    inquire(file=filename, exist=exists)
    if (.not.exists) then
        write(*,'(A)') "[ERROR]: File does not exist"
        call exit(1)
    end if

    n_pts = size(x)

    call gahm%initialize(filename, n_pts, x, y)
    call start_date%set(2005,8,27)
    !...Initialization of GAHM ends

    !...GAHM Time stepping loop
    j = 0
    do i = 0, 86400, 1800
        current_date = start_date%add(int(i, 8))
        call gahm%get(current_date, n_pts, u, v, p)

        j = j + 1

        !...Check the solution
        if (.not.is_equal(u(1), solution(j,1))) then
            write(*,'(A)') "[ERROR]: U values are not equal"
            write(*,'(A,F0.6)') "[ERROR]: u(1) = ", u(1)
            write(*,'(A,I0,A,F0.6)') "[ERROR]: solution(",j,",1) = ", solution(j,1)
            call exit(1)
        end if

        if (.not.is_equal(v(1), solution(j,2))) then
            write(*,'(A)') "[ERROR]: V values are not equal"
            write(*,'(A,F0.6)') "[ERROR]: v(1) = ", v(1)
            write(*,'(A,I0,A,F0.6)') "[ERROR]: solution(",j,",2) = ", solution(j,2)
            call exit(1)
        end if

        if (.not.is_equal(p(1), solution(j,3))) then
            write(*,'(A)') "[ERROR]: P values are not equal"
            write(*,'(A, F0.6)') "[ERROR]: p(1) = ", p(1)
            write(*,'(A,I0,A,F0.6)') "[ERROR]: solution(",j,",3) = ", solution(j,3)
            call exit(1)
        end if

!        write(*,'(F0.9,A,F0.9,A,F0.9,A)') u(1), ", ", v(1), ", ", p(1), ", "

    end do

end subroutine test_001