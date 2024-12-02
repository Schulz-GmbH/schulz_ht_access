<template>
	<!--  HEADER  -->
	<header-component title="HS - Wirelesss Access" :is-menu-open="isMenuOpen" @toggle-menu="handleToggleMenu"
		:is-notify-open="isNotifyOpen" @toggle-notify="handleToggleNotify" />
	<!--  MAIN  -->
	<section class="main-section h-full">
		<navbar-component :is-mobile="isMobile" :is-menu-open="isMenuOpen" :menu-items="menuItems" />

		<main class="main flex flex-col grow-[6] shrink-[1] basis-0 justify-between bg-background">
			<div class="main-panel p-3">
				<router-view />
			</div>
			<!--    FOOTER    -->
			<footer class="footer">
				<p class="footer__text">© 2024 Schulz Hygiene GmbH</p>
			</footer>
		</main>
	</section>
</template>

<script src="./index.layout.ts"></script>
