<template>
	<section class="app-wrapper relative h-[calc(100%-93px)]" :class="layoutClasses">
		<div v-if="layoutClasses.mobile && layoutClasses.openNavbar"
			class="drawer-bg absolute inset-0 w-full h-full bg-transparent z-[999]" @click="handleClickOutside" />
		<Sidebar />

		<main class="ease-soft-in-out xl:ml-68.5 relative h-full max-h-screen rounded-xl transition-all duration-200">
			<NavigationBar />
			<router-view />
		</main>
		<Footer />
	</section>
</template>

<script lang="ts">

export { default } from './app.layout'

</script>

<style src="./app.layout.scss"></style>
