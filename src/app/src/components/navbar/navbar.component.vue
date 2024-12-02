<template>
	<!-- MOBILE ASIDE MENU -->
	<transition @before-enter="beforeEnter" @enter="enter" @leave="leave" name="menu-slide">
		<aside v-if="isMobile && isMenuOpen" ref="mobileMenu"
			class="menu-mobile bg-white text-sm leading-normal flex-col z-[2] overflow-hidden">
			<template v-for="item in menuItems" :key="item.title">
				<template v-if="item.location">
					<RouterLink :to="item.location" class="menu-item relative flex items-center p-4 font-semibold">
						<div class="flex w-8 h-8 justify-center items-center">
							<i :class="item.icon"></i>
						</div>
						<span class="ml-4">{{ item.title }}</span>
					</RouterLink>
				</template>
				<template v-else>
					<div class="menu-item relative flex items-center p-4 font-semibold">
						<div class="flex w-8 h-8 justify-center items-center">
							<i :class="item.icon"></i>
						</div>
						<span class="ml-4">{{ item.title }}</span>
					</div>
				</template>
			</template>
		</aside>
	</transition>

	<!-- DESKTOP ASIDE MENU -->
	<transition name="menu-width">
		<aside v-if="!isMobile" class="menu bg-white text-sm leading-normal flex-col z-[2] flex"
			:class="{ 'menu-open': isMenuOpen, 'menu-closed': !isMenuOpen }">
			<template v-for="item in menuItems" :key="item.title">
				<template v-if="item.location">
					<RouterLink :to="item.location" class="menu-item relative flex items-center p-4 font-semibold">
						<div class="flex w-8 h-8 justify-center items-center">
							<i :class="item.icon"></i>
						</div>
						<span v-show="isMenuOpen" class="ml-4 menu-text">{{ item.title }}</span>
					</RouterLink>
				</template>
				<template v-else>
					<div class="menu-item relative flex items-center p-4 font-semibold">
						<div class="flex w-8 h-8 justify-center items-center">
							<i :class="item.icon"></i>
						</div>
						<span v-show="isMenuOpen" class="ml-4 menu-text">{{ item.title }}</span>
					</div>
				</template>
			</template>
		</aside>
	</transition>
</template>

<script src="./navbar.component.ts"></script>

<style scoped>
/* Transition for mobile slide (height animation) */
.menu-slide-enter-active,
.menu-slide-leave-active {
	overflow: hidden;
	/* Prevent overflow during animation */
}

/* Max height for mobile menu open */
.menu-mobile {
	transition: max-height 0.5s ease-in-out;
}

/* Transition for desktop width animation */
.menu-width-enter-active,
.menu-width-leave-active {
	transition: width 0.3s ease, opacity 0.3s ease;
}

.menu-width-enter,
.menu-width-leave-to {
	width: 4rem;
	opacity: 0;
}

.menu {
	transition: width 0.3s ease;
}

.menu-open {
	width: 16rem;
}

.menu-closed {
	width: 4rem;
}

/* Text should follow the width animation smoothly */
.menu-text {
	display: inline-block;
	white-space: nowrap;
	overflow: hidden;
	transition: width 0.3s ease, opacity 0.3s ease;
	width: auto;
	/* When menu is open, width is flexible */
}

.menu-closed .menu-text {
	width: 0;
	/* When menu is closed, text width is zero */
	opacity: 0;
}
</style>
