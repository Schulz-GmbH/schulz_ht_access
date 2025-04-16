<template>
	<div class="navbar">
		<div class="navbar-container w-full px-4 mx-auto flex flex-wrap items-center justify-between md:px-0">
			<div class="" :class="{ 'has-logo': isLogo }">
				<Logo v-if="isLogo" :collapse="isCollapse" />
				<h1>Navbar</h1>
			</div>
		</div>

	</div>
</template>

<script lang="ts">

export { default } from './navbar.components'

</script>
