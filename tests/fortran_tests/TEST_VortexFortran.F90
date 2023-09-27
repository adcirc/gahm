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
            -0.069293217d0, -0.762540532d0, 1012.502022216d0,&
            -0.083077802d0, -0.848962435d0, 1012.443810574d0,&
            -0.098710764d0, -0.940965460d0, 1012.381515450d0,&
            -0.116341586d0, -1.038673938d0, 1012.314994933d0,&
            -0.136124835d0, -1.142203874d0, 1012.244106459d0,&
            -0.158219900d0, -1.251661999d0, 1012.168707070d0,&
            -0.182790697d0, -1.367144854d0, 1012.088653662d0,&
            -0.210005326d0, -1.488737902d0, 1012.003803237d0,&
            -0.240035699d0, -1.616514694d0, 1011.914013151d0,&
            -0.273057128d0, -1.750536076d0, 1011.819141364d0,&
            -0.309247878d0, -1.890849448d0, 1011.719046683d0,&
            -0.348788692d0, -2.037488090d0, 1011.613589016d0,&
            -0.391862289d0, -2.190470546d0, 1011.502629618d0,&
            -0.410063221d0, -2.219882218d0, 1011.479199271d0,&
            -0.428727875d0, -2.249137561d0, 1011.455857596d0,&
            -0.447856348d0, -2.278203312d0, 1011.432626386d0,&
            -0.467447809d0, -2.307044531d0, 1011.409528579d0,&
            -0.487500423d0, -2.335624543d0, 1011.386588308d0,&
            -0.508011260d0, -2.363904879d0, 1011.363830960d0,&
            -0.528976208d0, -2.391845208d0, 1011.341283232d0,&
            -0.550389874d0, -2.419403267d0, 1011.318973197d0,&
            -0.572245482d0, -2.446534799d0, 1011.296930363d0,&
            -0.594534759d0, -2.473193474d0, 1011.275185748d0,&
            -0.617247820d0, -2.499330815d0, 1011.253771952d0,&
            -0.640373042d0, -2.524896121d0, 1011.232723234d0,&
            -0.652362348d0, -2.521564120d0, 1011.251889842d0,&
            -0.664539315d0, -2.518480063d0, 1011.270713730d0,&
            -0.676909568d0, -2.515640151d0, 1011.289202836d0,&
            -0.689478808d0, -2.513040508d0, 1011.307364966d0,&
            -0.702277714d0, -2.510761561d0, 1011.325178327d0,&
            -0.715250451d0, -2.508589458d0, 1011.342723580d0,&
            -0.728443366d0, -2.506658679d0, 1011.359960117d0,&
            -0.741862847d0, -2.504965582d0, 1011.376894874d0,&
            -0.755515383d0, -2.503506426d0, 1011.393534681d0,&
            -0.769407559d0, -2.502277361d0, 1011.409886267d0,&
            -0.783546059d0, -2.501274421d0, 1011.425956263d0,&
            -0.797937665d0, -2.500493512d0, 1011.441751206d0,&
            -0.831361427d0, -2.585591876d0, 1011.376290720d0,&
            -0.866155515d0, -2.673238348d0, 1011.308281392d0,&
            -0.902386787d0, -2.763527341d0, 1011.237610137d0,&
            -0.940110631d0, -2.856512030d0, 1011.164180436d0,&
            -0.979384165d0, -2.952244572d0, 1011.087892722d0,&
            -1.020266257d0, -3.050775903d0, 1011.008644316d0,&
            -1.062817544d0, -3.152155530d0, 1010.926329371d0,&
            -1.107100448d0, -3.256431312d0, 1010.840838817d0,&
            -1.153179189d0, -3.363649225d0, 1010.752060305d0,&
            -1.201119793d0, -3.473853115d0, 1010.659878164d0,&
            -1.250990104d0, -3.587084442d0, 1010.564173356d0,&
            -1.302859781d0, -3.703382010d0, 1010.464823439d0 &
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

        !write(*,'(F0.9,A,F0.9,A,F0.9,A)') u(1), ", ", v(1), ", ", p(1), ", "

    end do

end subroutine test_001